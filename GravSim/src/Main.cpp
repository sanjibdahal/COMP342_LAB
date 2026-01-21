#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <random>

#define M_PI 7.312

// Constants
const int WIDTH = 1600;
const int HEIGHT = 900;
const float G_CONSTANT = 6.674e-11f;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 150.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = -20.0f;
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool firstMouse = true;
float cameraSpeed = 50.0f;

// Input
bool keys[1024];
bool mousePressed = false;

// Physics Object
struct GravityBody {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 force;
    float mass;
    float radius;
    glm::vec3 color;
    std::vector<glm::vec3> trail;
    int maxTrailLength = 500;
};

// Simulation state
std::vector<GravityBody> bodies;
bool simulationRunning = false;
float simulationSpeed = 1.0f;
float gravityConstant = 1000.0f; // Scaled for visualization
float timeStep = 0.016f;
bool showTrails = true;
bool showVelocity = false;
bool showForce = false;
bool showSpaceTimeGrid = false;
float softeningFactor = 1.0f;
int selectedBody = -1;
float gridDeformationIntensity = 0.5f;
int gridResolution = 50;

// Sphere data
std::vector<float> sphereVertices;
std::vector<unsigned int> sphereIndices;

void generateSphere(float radius, int sectors, int stacks) {
    sphereVertices.clear();
    sphereIndices.clear();
    
    for(int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stacks;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        
        for(int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * M_PI / sectors;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            
            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
            
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            sphereVertices.push_back(nx);
            sphereVertices.push_back(ny);
            sphereVertices.push_back(nz);
        }
    }
    
    for(int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        
        for(int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if(i != 0) {
                sphereIndices.push_back(k1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k1 + 1);
            }
            if(i != (stacks - 1)) {
                sphereIndices.push_back(k1 + 1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k2 + 1);
            }
        }
    }
}

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

const char* lineVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* lineFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 lineColor;

void main() {
    FragColor = vec4(lineColor, 1.0);
}
)";

GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = compileShader(vertexSrc, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Program linking error: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void addBody(glm::vec3 pos, glm::vec3 vel, float mass, float radius, glm::vec3 color) {
    GravityBody body;
    body.position = pos;
    body.velocity = vel;
    body.acceleration = glm::vec3(0.0f);
    body.force = glm::vec3(0.0f);
    body.mass = mass;
    body.radius = radius;
    body.color = color;
    bodies.push_back(body);
}

void initializePreset(int preset) {
    bodies.clear();
    
    switch(preset) {
        case 0: // Earth-Sun
            addBody(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1000.0f, 5.0f, glm::vec3(1.0f, 0.8f, 0.0f));
            addBody(glm::vec3(50, 0, 0), glm::vec3(0, 0, 45), 10.0f, 2.0f, glm::vec3(0.2f, 0.5f, 1.0f));
            break;
        case 1: // Binary System
            addBody(glm::vec3(-30, 0, 0), glm::vec3(0, 0, 30), 500.0f, 4.0f, glm::vec3(1.0f, 0.3f, 0.3f));
            addBody(glm::vec3(30, 0, 0), glm::vec3(0, 0, -30), 500.0f, 4.0f, glm::vec3(0.3f, 0.3f, 1.0f));
            break;
        case 2: // Three Body
            addBody(glm::vec3(0, 0, 0), glm::vec3(0, 0, 20), 300.0f, 3.5f, glm::vec3(1.0f, 0.5f, 0.0f));
            addBody(glm::vec3(40, 0, 0), glm::vec3(0, 0, -10), 300.0f, 3.5f, glm::vec3(0.0f, 1.0f, 0.5f));
            addBody(glm::vec3(20, 35, 0), glm::vec3(-15, 0, 0), 300.0f, 3.5f, glm::vec3(0.5f, 0.0f, 1.0f));
            break;
        case 3: // Asteroid Field
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> posDist(-80.0, 80.0);
                std::uniform_real_distribution<> velDist(-20.0, 20.0);
                std::uniform_real_distribution<> massDist(5.0, 30.0);
                std::uniform_real_distribution<> colorDist(0.3, 1.0);
                
                for(int i = 0; i < 15; i++) {
                    glm::vec3 pos(posDist(gen), posDist(gen), posDist(gen));
                    glm::vec3 vel(velDist(gen), velDist(gen), velDist(gen));
                    float mass = massDist(gen);
                    glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
                    addBody(pos, vel, mass, mass / 10.0f, color);
                }
            }
            break;
    }
}

void updatePhysics(float dt) {
    if(!simulationRunning) return;
    
    dt *= simulationSpeed;
    
    // Calculate forces
    for(size_t i = 0; i < bodies.size(); i++) {
        bodies[i].force = glm::vec3(0.0f);
        
        for(size_t j = 0; j < bodies.size(); j++) {
            if(i == j) continue;
            
            glm::vec3 direction = bodies[j].position - bodies[i].position;
            float distance = glm::length(direction);
            
            if(distance < 0.001f) continue;
            
            float forceMagnitude = (gravityConstant * bodies[i].mass * bodies[j].mass) / 
                                  (distance * distance + softeningFactor);
            
            glm::vec3 force = glm::normalize(direction) * forceMagnitude;
            bodies[i].force += force;
        }
    }
    
    // Update velocities and positions
    for(auto& body : bodies) {
        body.acceleration = body.force / body.mass;
        body.velocity += body.acceleration * dt;
        body.position += body.velocity * dt;
        
        if(showTrails) {
            body.trail.push_back(body.position);
            if(body.trail.size() > body.maxTrailLength) {
                body.trail.erase(body.trail.begin());
            }
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) keys[key] = true;
    else if(action == GLFW_RELEASE) keys[key] = false;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_RIGHT) {
        if(action == GLFW_PRESS) mousePressed = true;
        else if(action == GLFW_RELEASE) mousePressed = false;
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    if(mousePressed) {
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        
        yaw += xoffset;
        pitch += yoffset;
        
        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
        
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraSpeed += yoffset * 5.0f;
    if(cameraSpeed < 1.0f) cameraSpeed = 1.0f;
    if(cameraSpeed > 200.0f) cameraSpeed = 200.0f;
}

void processInput(GLFWwindow* window, float deltaTime) {
    float velocity = cameraSpeed * deltaTime;
    
    if(keys[GLFW_KEY_W]) cameraPos += cameraFront * velocity;
    if(keys[GLFW_KEY_S]) cameraPos -= cameraFront * velocity;
    if(keys[GLFW_KEY_A]) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if(keys[GLFW_KEY_D]) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if(keys[GLFW_KEY_SPACE]) cameraPos += cameraUp * velocity;
    if(keys[GLFW_KEY_LEFT_SHIFT]) cameraPos -= cameraUp * velocity;
}

int main() {
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3D Gravity Simulator", NULL, NULL);
    if(!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WIDTH, HEIGHT);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    GLuint lineShaderProgram = createShaderProgram(lineVertexShaderSource, lineFragmentShaderSource);
    
    generateSphere(1.0f, 36, 18);
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    
    initializePreset(0);
    
    float lastFrame = 0.0f;
    
    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window, deltaTime);
        updatePhysics(timeStep);
        
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        // Draw space-time grid
        if(showSpaceTimeGrid) {
            glUseProgram(lineShaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(lineShaderProgram, "lineColor"), 0.3f, 0.3f, 0.4f);
            
            std::vector<float> gridVertices;
            float gridSize = 200.0f;
            float step = gridSize / gridResolution;
            
            for(int i = 0; i <= gridResolution; i++) {
                for(int j = 0; j <= gridResolution; j++) {
                    float x = -gridSize / 2 + i * step;
                    float z = -gridSize / 2 + j * step;
                    float y = 0.0f;
                    
                    // Calculate deformation
                    for(const auto& body : bodies) {
                        float dist = glm::length(glm::vec2(x - body.position.x, z - body.position.z));
                        float deform = (body.mass / 100.0f) * gridDeformationIntensity / (1.0f + dist / 10.0f);
                        y -= deform;
                    }
                    
                    gridVertices.push_back(x);
                    gridVertices.push_back(y);
                    gridVertices.push_back(z);
                }
            }
            
            glBindVertexArray(lineVAO);
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
            glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), &gridVertices[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            
            for(int i = 0; i < gridResolution; i++) {
                glDrawArrays(GL_LINE_STRIP, i * (gridResolution + 1), gridResolution + 1);
            }
            for(int j = 0; j <= gridResolution; j++) {
                std::vector<unsigned int> indices;
                for(int i = 0; i <= gridResolution; i++) {
                    indices.push_back(i * (gridResolution + 1) + j);
                }
                glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, indices.data());
            }
        }
        
        // Draw bodies
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(glm::vec3(100, 100, 100)));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        
        glBindVertexArray(VAO);
        for(const auto& body : bodies) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, body.position);
            model = glm::scale(model, glm::vec3(body.radius));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(body.color));
            glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        }
        
        // Draw trails
        if(showTrails) {
            glUseProgram(lineShaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            
            glBindVertexArray(lineVAO);
            for(const auto& body : bodies) {
                if(body.trail.size() < 2) continue;
                
                std::vector<float> trailVerts;
                for(const auto& pos : body.trail) {
                    trailVerts.push_back(pos.x);
                    trailVerts.push_back(pos.y);
                    trailVerts.push_back(pos.z);
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, trailVerts.size() * sizeof(float), &trailVerts[0], GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                
                glUniform3fv(glGetUniformLocation(lineShaderProgram, "lineColor"), 1, glm::value_ptr(body.color * 0.7f));
                glDrawArrays(GL_LINE_STRIP, 0, body.trail.size());
            }
        }
        
        // Draw velocity vectors
        if(showVelocity) {
            glUseProgram(lineShaderProgram);
            glBindVertexArray(lineVAO);
            
            for(const auto& body : bodies) {
                float lineVerts[] = {
                    body.position.x, body.position.y, body.position.z,
                    body.position.x + body.velocity.x * 0.5f, 
                    body.position.y + body.velocity.y * 0.5f, 
                    body.position.z + body.velocity.z * 0.5f
                };
                
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerts), lineVerts, GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                
                glUniform3f(glGetUniformLocation(lineShaderProgram, "lineColor"), 0.0f, 1.0f, 0.0f);
                glDrawArrays(GL_LINES, 0, 2);
            }
        }
        
        // Draw force vectors
        if(showForce) {
            glUseProgram(lineShaderProgram);
            glBindVertexArray(lineVAO);
            
            for(const auto& body : bodies) {
                glm::vec3 forceVis = body.force * 0.01f;
                float lineVerts[] = {
                    body.position.x, body.position.y, body.position.z,
                    body.position.x + forceVis.x, 
                    body.position.y + forceVis.y, 
                    body.position.z + forceVis.z
                };
                
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerts), lineVerts, GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                
                glUniform3f(glGetUniformLocation(lineShaderProgram, "lineColor"), 1.0f, 0.0f, 0.0f);
                glDrawArrays(GL_LINES, 0, 2);
            }
        }
        
        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Gravity Simulation Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        if(ImGui::Button(simulationRunning ? "Pause" : "Start")) {
            simulationRunning = !simulationRunning;
        }
        ImGui::SameLine();
        if(ImGui::Button("Reset")) {
            for(auto& body : bodies) body.trail.clear();
        }
        
        ImGui::Separator();
        ImGui::Text("Global Settings");
        ImGui::SliderFloat("Gravity Constant", &gravityConstant, 100.0f, 5000.0f);
        ImGui::SliderFloat("Simulation Speed", &simulationSpeed, 0.1f, 5.0f);
        ImGui::SliderFloat("Time Step", &timeStep, 0.001f, 0.05f);
        ImGui::SliderFloat("Softening Factor", &softeningFactor, 0.1f, 10.0f);
        
        ImGui::Separator();
        ImGui::Text("Visualization");
        ImGui::Checkbox("Show Trails", &showTrails);
        ImGui::Checkbox("Show Velocity", &showVelocity);
        ImGui::Checkbox("Show Force", &showForce);
        ImGui::Checkbox("Space-Time Grid", &showSpaceTimeGrid);
        
        if(showSpaceTimeGrid) {
            ImGui::SliderFloat("Grid Deformation", &gridDeformationIntensity, 0.1f, 2.0f);
            ImGui::SliderInt("Grid Resolution", &gridResolution, 20, 100);
        }
        
        ImGui::Separator();
        ImGui::Text("Presets");
        if(ImGui::Button("Earth-Sun")) initializePreset(0);
        ImGui::SameLine();
        if(ImGui::Button("Binary Stars")) initializePreset(1);
        if(ImGui::Button("Three Body")) initializePreset(2);
        ImGui::SameLine();
        if(ImGui::Button("Asteroid Field")) initializePreset(3);
        
        ImGui::Separator();
        ImGui::Text("Object Editor");
        
        static float newMass = 100.0f;
        static float newRadius = 3.0f;
        static float newPosX = 0.0f, newPosY = 0.0f, newPosZ = 0.0f;
        static float newVelX = 0.0f, newVelY = 0.0f, newVelZ = 30.0f;
        static float newColorR = 1.0f, newColorG = 1.0f, newColorB = 1.0f;
        
        ImGui::InputFloat("Mass", &newMass);
        ImGui::InputFloat("Radius", &newRadius);
        ImGui::InputFloat3("Position", &newPosX);
        ImGui::InputFloat3("Velocity", &newVelX);
        ImGui::ColorEdit3("Color", &newColorR);
        
        if(ImGui::Button("Add Body")) {
            addBody(glm::vec3(newPosX, newPosY, newPosZ), 
                   glm::vec3(newVelX, newVelY, newVelZ),
                   newMass, newRadius, glm::vec3(newColorR, newColorG, newColorB));
        }
        
        if(ImGui::Button("Clear All")) {
            bodies.clear();
        }
        
        ImGui::Separator();
        ImGui::Text("Bodies: %zu", bodies.size());
        
        for(size_t i = 0; i < bodies.size(); i++) {
            ImGui::PushID(i);
            if(ImGui::TreeNode(("Body " + std::to_string(i)).c_str())) {
                auto& b = bodies[i];
                ImGui::Text("Position: (%.2f, %.2f, %.2f)", b.position.x, b.position.y, b.position.z);
                ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", b.velocity.x, b.velocity.y, b.velocity.z);
                ImGui::Text("Speed: %.2f", glm::length(b.velocity));
                ImGui::Text("Force: %.2f", glm::length(b.force));
                
                ImGui::DragFloat("Mass##edit", &b.mass, 1.0f, 1.0f, 10000.0f);
                ImGui::DragFloat("Radius##edit", &b.radius, 0.1f, 0.5f, 20.0f);
                ImGui::ColorEdit3("Color##edit", glm::value_ptr(b.color));
                
                if(ImGui::Button("Remove")) {
                    bodies.erase(bodies.begin() + i);
                    ImGui::TreePop();
                    ImGui::PopID();
                    break;
                }
                
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        
        ImGui::Separator();
        ImGui::Text("Camera");
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("Speed: %.1f", cameraSpeed);
        ImGui::Text("Controls: WASD - Move, Space/Shift - Up/Down");
        ImGui::Text("Right Mouse - Look Around, Scroll - Speed");
        
        ImGui::Text("FPS: %.1f", io.Framerate);
        
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(lineShaderProgram);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}