#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

const int M_PI = 3.14;

// Shader sources embedded
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
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * objectColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * objectColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    vec3 result = ambient + diffuse + specular;
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

Renderer::Renderer() : showTrails(true), showVelocity(false), 
                       showForces(false), showGrid(true), showAxes(true) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize() {
    // Initialize shaders
    if (!bodyShader.loadFromString(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to load body shader" << std::endl;
        return false;
    }
    
    if (!lineShader.loadFromString(lineVertexShaderSource, lineFragmentShaderSource)) {
        std::cerr << "Failed to load line shader" << std::endl;
        return false;
    }
    
    // Create sphere mesh
    createSphere(1.0f, 30, 30);
    
    // Create grid
    createGrid(200.0f, 20);
    
    // Create axes
    createAxes();
    
    // Setup line VAO
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    
    return true;
}

void Renderer::cleanup() {
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &axesVAO);
    glDeleteBuffers(1, &axesVBO);
    
    bodyShader.destroy();
    lineShader.destroy();
}

void Renderer::render(const std::vector<std::unique_ptr<Body>>& bodies,
                     const Camera& camera, float aspectRatio) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
    
    // Render grid and axes
    if (showGrid) renderGrid(view, projection);
    if (showAxes) renderAxes(view, projection);
    
    // Render bodies
    renderBodies(bodies, view, projection);
    
    // Render trails
    if (showTrails) renderTrails(bodies, view, projection);
    
    // Render vectors
    if (showVelocity || showForces) renderVectors(bodies, view, projection);
}

void Renderer::renderBodies(const std::vector<std::unique_ptr<Body>>& bodies,
                           const glm::mat4& view, const glm::mat4& projection) {
    bodyShader.use();
    bodyShader.setMat4("view", view);
    bodyShader.setMat4("projection", projection);
    
    // Simple lighting from above
    bodyShader.setVec3("lightPos", glm::vec3(0.0f, 1000.0f, 0.0f));
    bodyShader.setVec3("viewPos", glm::vec3(0.0f, 50.0f, 100.0f));
    
    for (const auto& body : bodies) {
        renderSphere(body->position, body->radius, body->color, view, projection);
    }
}

void Renderer::renderSphere(const glm::vec3& position, float radius,
                           const glm::vec3& color, const glm::mat4& view,
                           const glm::mat4& projection) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius));
    
    bodyShader.use();
    bodyShader.setMat4("model", model);
    bodyShader.setVec3("objectColor", color);
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Continues in Part 2...
// Continuation of Renderer.cpp

void Renderer::createSphere(float radius, unsigned int rings, unsigned int sectors) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    
    for (unsigned int r = 0; r < rings; r++) {
        for (unsigned int s = 0; s < sectors; s++) {
            float const y = sin(-M_PI / 2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);
            
            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    
    for (unsigned int r = 0; r < rings - 1; r++) {
        for (unsigned int s = 0; s < sectors - 1; s++) {
            indices.push_back(r * sectors + s);
            indices.push_back(r * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + (s + 1));
            
            indices.push_back(r * sectors + s);
            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + s);
        }
    }
    
    sphereIndexCount = indices.size();
    
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    
    glBindVertexArray(sphereVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::renderTrails(const std::vector<std::unique_ptr<Body>>& bodies,
                           const glm::mat4& view, const glm::mat4& projection) {
    lineShader.use();
    lineShader.setMat4("view", view);
    lineShader.setMat4("projection", projection);
    
    for (const auto& body : bodies) {
        if (body->trail.size() < 2) continue;
        
        lineShader.setVec3("lineColor", body->color * 0.7f);
        
        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, body->trail.size() * sizeof(glm::vec3),
                     body->trail.data(), GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        glDrawArrays(GL_LINE_STRIP, 0, body->trail.size());
        glBindVertexArray(0);
    }
}

void Renderer::renderVectors(const std::vector<std::unique_ptr<Body>>& bodies,
                            const glm::mat4& view, const glm::mat4& projection) {
    for (const auto& body : bodies) {
        if (showVelocity && glm::length(body->velocity) > 0.001f) {
            glm::vec3 end = body->position + glm::normalize(body->velocity) * 5.0f;
            renderLine(body->position, end, glm::vec3(0.0f, 1.0f, 0.0f), view, projection);
        }
        
        if (showForces && glm::length(body->force) > 0.001f) {
            glm::vec3 end = body->position + glm::normalize(body->force) * 5.0f;
            renderLine(body->position, end, glm::vec3(1.0f, 0.0f, 0.0f), view, projection);
        }
    }
}

void Renderer::renderLine(const glm::vec3& start, const glm::vec3& end,
                         const glm::vec3& color, const glm::mat4& view,
                         const glm::mat4& projection) {
    float vertices[] = {
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };
    
    lineShader.use();
    lineShader.setMat4("view", view);
    lineShader.setMat4("projection", projection);
    lineShader.setVec3("lineColor", color);
    
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Renderer::createGrid(float size, int divisions) {
    std::vector<float> vertices;
    float step = size / divisions;
    float half = size / 2.0f;
    
    for (int i = 0; i <= divisions; i++) {
        float pos = -half + i * step;
        vertices.push_back(pos); vertices.push_back(0.0f); vertices.push_back(-half);
        vertices.push_back(pos); vertices.push_back(0.0f); vertices.push_back(half);
        vertices.push_back(-half); vertices.push_back(0.0f); vertices.push_back(pos);
        vertices.push_back(half); vertices.push_back(0.0f); vertices.push_back(pos);
    }
    
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::renderGrid(const glm::mat4& view, const glm::mat4& projection) {
    lineShader.use();
    lineShader.setMat4("view", view);
    lineShader.setMat4("projection", projection);
    lineShader.setVec3("lineColor", glm::vec3(0.2f, 0.2f, 0.2f));
    
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, 84);
    glBindVertexArray(0);
}

void Renderer::createAxes() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,  20.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 20.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 20.0f
    };
    
    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);
    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::renderAxes(const glm::mat4& view, const glm::mat4& projection) {
    lineShader.use();
    lineShader.setMat4("view", view);
    lineShader.setMat4("projection", projection);
    
    glBindVertexArray(axesVAO);
    lineShader.setVec3("lineColor", glm::vec3(1.0f, 0.0f, 0.0f));
    glDrawArrays(GL_LINES, 0, 2);
    lineShader.setVec3("lineColor", glm::vec3(0.0f, 1.0f, 0.0f));
    glDrawArrays(GL_LINES, 2, 2);
    lineShader.setVec3("lineColor", glm::vec3(0.0f, 0.0f, 1.0f));
    glDrawArrays(GL_LINES, 4, 2);
    glBindVertexArray(0);
}