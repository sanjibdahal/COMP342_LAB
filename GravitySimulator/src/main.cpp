#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#include "Body.h"
#include "PhysicsEngine.h"
#include "Camera.h"
#include "Renderer.h"
#include "UI.h"

const int M_PI = 3.14;

// Global state
Camera camera(glm::vec3(0.0f, 50.0f, 150.0f), glm::vec3(0.0f));
Renderer renderer;
PhysicsEngine physics(1.0e10f);
std::vector<std::unique_ptr<Body>> bodies;

bool paused = false;
float timeScale = 1.0f;
bool leftMousePressed = false;
bool rightMousePressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Function declarations
void setupSimulation(UI::SimulationMode mode);
void setupSolarSystem();
void setupThreeBodyProblem();
void setupBinaryStarSystem();
void setupFigure8Orbit();
void setupGalaxyCollision();
void setupSandboxMode();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                         "Gravity Simulator - N-Body Physics", 
                                         NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
    
    // OpenGL configuration
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    
    // Initialize renderer
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Setup initial simulation
    setupSimulation(UI::SimulationMode::SOLAR_SYSTEM);
    
    // Create UI
    UI ui(window);
    
    // Timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    std::cout << "\n=== GRAVITY SIMULATOR ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  1-6: Switch simulation modes" << std::endl;
    std::cout << "  SPACE: Pause/Resume" << std::endl;
    std::cout << "  R: Reset simulation" << std::endl;
    std::cout << "  T: Toggle trails" << std::endl;
    std::cout << "  V: Toggle velocity vectors" << std::endl;
    std::cout << "  F: Toggle force vectors" << std::endl;
    std::cout << "  +/-: Adjust time scale" << std::endl;
    std::cout << "  Mouse: Rotate/Pan camera" << std::endl;
    std::cout << "  Scroll: Zoom" << std::endl;
    std::cout << "========================\n" << std::endl;
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Process input
        glfwPollEvents();
        
        // Update physics
        if (!paused) {
            physics.update(bodies, deltaTime * timeScale);
        }
        
        // Update camera
        camera.update(deltaTime);
        
        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspectRatio = (float)width / (float)height;
        
        renderer.render(bodies, camera, aspectRatio);
        ui.render(bodies, deltaTime, timeScale, paused);
        
        // Check if new simulation mode selected
        if (ui.hasNewModeSelected()) {
            setupSimulation(ui.getSelectedMode());
            ui.resetModeSelection();
        }
        
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    renderer.cleanup();
    glfwTerminate();
    return 0;
}

// Continued in Part 2...
// Continuation of main.cpp - Simulation setup functions

void setupSimulation(UI::SimulationMode mode) {
    bodies.clear();
    
    switch (mode) {
        case UI::SimulationMode::SOLAR_SYSTEM:
            setupSolarSystem();
            std::cout << "Loaded: Solar System" << std::endl;
            break;
        case UI::SimulationMode::THREE_BODY:
            setupThreeBodyProblem();
            std::cout << "Loaded: Three-Body Problem" << std::endl;
            break;
        case UI::SimulationMode::BINARY_STAR:
            setupBinaryStarSystem();
            std::cout << "Loaded: Binary Star System" << std::endl;
            break;
        case UI::SimulationMode::FIGURE_8:
            setupFigure8Orbit();
            std::cout << "Loaded: Figure-8 Orbit" << std::endl;
            break;
        case UI::SimulationMode::GALAXY_COLLISION:
            setupGalaxyCollision();
            std::cout << "Loaded: Galaxy Collision" << std::endl;
            break;
        case UI::SimulationMode::SANDBOX:
            setupSandboxMode();
            std::cout << "Loaded: Sandbox Mode" << std::endl;
            break;
    }
}

void setupSolarSystem() {
    // Sun (massive, fixed at center)
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 0.0f),           // position
        glm::vec3(0.0f, 0.0f, 0.0f),           // velocity
        5000.0f,                                // mass
        5.0f,                                   // radius
        glm::vec3(1.0f, 0.9f, 0.2f),           // color (yellow)
        "Sun",
        true                                    // fixed
    ));
    
    // Mercury
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(30.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 18.0f),
        10.0f,
        1.0f,
        glm::vec3(0.7f, 0.7f, 0.7f),
        "Mercury"
    ));
    
    // Venus
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(45.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 15.0f),
        20.0f,
        1.5f,
        glm::vec3(0.9f, 0.7f, 0.4f),
        "Venus"
    ));
    
    // Earth
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(60.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 13.0f),
        25.0f,
        1.8f,
        glm::vec3(0.2f, 0.4f, 0.8f),
        "Earth"
    ));
    
    // Mars
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(80.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 11.0f),
        15.0f,
        1.2f,
        glm::vec3(0.8f, 0.3f, 0.2f),
        "Mars"
    ));
    
    // Jupiter
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(120.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 8.5f),
        100.0f,
        3.5f,
        glm::vec3(0.8f, 0.7f, 0.6f),
        "Jupiter"
    ));
}

void setupThreeBodyProblem() {
    // Classic three-body problem - chaotic system
    // Using slightly asymmetric initial conditions for interesting dynamics
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-30.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 8.0f),
        100.0f,
        3.0f,
        glm::vec3(1.0f, 0.2f, 0.2f),
        "Body 1"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(30.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -8.0f),
        100.0f,
        3.0f,
        glm::vec3(0.2f, 1.0f, 0.2f),
        "Body 2"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 40.0f),
        glm::vec3(-6.0f, 0.0f, 0.0f),
        100.0f,
        3.0f,
        glm::vec3(0.2f, 0.2f, 1.0f),
        "Body 3"
    ));
}

void setupBinaryStarSystem() {
    // Two stars orbiting each other with planets
    
    // Star 1 (Blue)
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-40.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 6.0f),
        2000.0f,
        4.0f,
        glm::vec3(0.3f, 0.5f, 1.0f),
        "Star A"
    ));
    
    // Star 2 (Red)
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(40.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -6.0f),
        2000.0f,
        4.0f,
        glm::vec3(1.0f, 0.3f, 0.3f),
        "Star B"
    ));
    
    // Planet orbiting Star 1
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-60.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 12.0f),
        20.0f,
        1.5f,
        glm::vec3(0.5f, 0.8f, 0.5f),
        "Planet A1"
    ));
    
    // Planet orbiting Star 2
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(60.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -12.0f),
        20.0f,
        1.5f,
        glm::vec3(0.8f, 0.5f, 0.8f),
        "Planet B1"
    ));
    
    // Circumbinary planet (orbits both stars)
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 100.0f),
        glm::vec3(7.0f, 0.0f, 0.0f),
        15.0f,
        1.2f,
        glm::vec3(0.7f, 0.7f, 0.3f),
        "Circumbinary"
    ));
}

void setupFigure8Orbit() {
    // Famous figure-8 stable three-body orbit
    // These initial conditions produce a stable figure-8 pattern
    
    float m = 100.0f;
    float v = 5.5f;
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-25.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, v),
        m,
        3.0f,
        glm::vec3(1.0f, 0.3f, 0.3f),
        "Body 1"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(25.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, v),
        m,
        3.0f,
        glm::vec3(0.3f, 1.0f, 0.3f),
        "Body 2"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -2.0f * v),
        m,
        3.0f,
        glm::vec3(0.3f, 0.3f, 1.0f),
        "Body 3"
    ));
}

void setupGalaxyCollision() {
    // Simulate two small galaxies colliding
    // Each "galaxy" is a central massive body with several orbiting bodies
    
    // Galaxy 1 center
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-80.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 3.0f),
        3000.0f,
        5.0f,
        glm::vec3(0.9f, 0.8f, 0.3f),
        "Galaxy 1 Core"
    ));
    
    // Galaxy 1 stars
    for (int i = 0; i < 8; i++) {
        float angle = i * 2.0f * M_PI / 8.0f;
        float radius = 20.0f + (i % 3) * 10.0f;
        float speed = 5.0f + (i % 3) * 2.0f;
        
        bodies.push_back(std::make_unique<Body>(
            glm::vec3(-80.0f + radius * cos(angle), 0.0f, radius * sin(angle)),
            glm::vec3(-speed * sin(angle), 0.0f, 3.0f + speed * cos(angle)),
            15.0f,
            1.0f,
            glm::vec3(0.8f, 0.8f, 0.9f),
            "G1-Star" + std::to_string(i)
        ));
    }
    
    // Galaxy 2 center
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(80.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -3.0f),
        3000.0f,
        5.0f,
        glm::vec3(0.3f, 0.6f, 0.9f),
        "Galaxy 2 Core"
    ));
    
    // Galaxy 2 stars
    for (int i = 0; i < 8; i++) {
        float angle = i * 2.0f * M_PI / 8.0f;
        float radius = 20.0f + (i % 3) * 10.0f;
        float speed = 5.0f + (i % 3) * 2.0f;
        
        bodies.push_back(std::make_unique<Body>(
            glm::vec3(80.0f + radius * cos(angle), 0.0f, radius * sin(angle)),
            glm::vec3(-speed * sin(angle), 0.0f, -3.0f + speed * cos(angle)),
            15.0f,
            1.0f,
            glm::vec3(0.9f, 0.9f, 1.0f),
            "G2-Star" + std::to_string(i)
        ));
    }
}

void setupSandboxMode() {
    // Empty scene - user can add bodies interactively
    // Add just one central body to start
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        1000.0f,
        4.0f,
        glm::vec3(1.0f, 1.0f, 0.5f),
        "Central Body",
        true
    ));
}

// Continued in Part 3...
// Continuation of main.cpp - Input callback functions

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
                
            case GLFW_KEY_SPACE:
                paused = !paused;
                std::cout << (paused ? "Paused" : "Resumed") << std::endl;
                break;
                
            case GLFW_KEY_R:
                // Reset - reload current simulation
                std::cout << "Resetting simulation..." << std::endl;
                for (auto& body : bodies) {
                    body->clearTrail();
                }
                break;
                
            case GLFW_KEY_T:
                renderer.toggleTrails();
                std::cout << "Trails: " << (renderer.isShowingTrails() ? "ON" : "OFF") << std::endl;
                break;
                
            case GLFW_KEY_V:
                renderer.toggleVelocityVectors();
                std::cout << "Velocity vectors: " << (renderer.isShowingVelocity() ? "ON" : "OFF") << std::endl;
                break;
                
            case GLFW_KEY_F:
                renderer.toggleForceVectors();
                std::cout << "Force vectors: " << (renderer.isShowingForces() ? "ON" : "OFF") << std::endl;
                break;
                
            case GLFW_KEY_G:
                renderer.toggleGrid();
                break;
                
            case GLFW_KEY_A:
                renderer.toggleAxes();
                break;
                
            case GLFW_KEY_EQUAL:  // + key
            case GLFW_KEY_KP_ADD:
                timeScale *= 1.5f;
                std::cout << "Time scale: " << timeScale << "x" << std::endl;
                break;
                
            case GLFW_KEY_MINUS:
            case GLFW_KEY_KP_SUBTRACT:
                timeScale /= 1.5f;
                if (timeScale < 0.1f) timeScale = 0.1f;
                std::cout << "Time scale: " << timeScale << "x" << std::endl;
                break;
                
            // Simulation mode selection
            case GLFW_KEY_1:
                setupSimulation(UI::SimulationMode::SOLAR_SYSTEM);
                break;
                
            case GLFW_KEY_2:
                setupSimulation(UI::SimulationMode::THREE_BODY);
                break;
                
            case GLFW_KEY_3:
                setupSimulation(UI::SimulationMode::BINARY_STAR);
                break;
                
            case GLFW_KEY_4:
                setupSimulation(UI::SimulationMode::FIGURE_8);
                break;
                
            case GLFW_KEY_5:
                setupSimulation(UI::SimulationMode::GALAXY_COLLISION);
                break;
                
            case GLFW_KEY_6:
                setupSimulation(UI::SimulationMode::SANDBOX);
                break;
                
            case GLFW_KEY_N:
                // Add new random body in sandbox mode
                {
                    float angle = (rand() % 360) * M_PI / 180.0f;
                    float radius = 40.0f + (rand() % 60);
                    float speed = 8.0f + (rand() % 5);
                    
                    bodies.push_back(std::make_unique<Body>(
                        glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle)),
                        glm::vec3(-speed * sin(angle), 0.0f, speed * cos(angle)),
                        20.0f + (rand() % 80),
                        1.0f + (rand() % 3),
                        glm::vec3((rand() % 100) / 100.0f, 
                                 (rand() % 100) / 100.0f, 
                                 (rand() % 100) / 100.0f),
                        "Body " + std::to_string(bodies.size())
                    ));
                    std::cout << "Added new body. Total: " << bodies.size() << std::endl;
                }
                break;
                
            case GLFW_KEY_DELETE:
            case GLFW_KEY_BACKSPACE:
                // Remove selected body
                for (auto it = bodies.begin(); it != bodies.end(); ++it) {
                    if ((*it)->isSelected && !(*it)->isFixed) {
                        std::cout << "Removed " << (*it)->name << std::endl;
                        bodies.erase(it);
                        break;
                    }
                }
                break;
                
            case GLFW_KEY_C:
                // Clear all trails
                for (auto& body : bodies) {
                    body->clearTrail();
                }
                std::cout << "Cleared all trails" << std::endl;
                break;
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMousePressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else {
            leftMousePressed = false;
        }
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMousePressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else {
            rightMousePressed = false;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (leftMousePressed) {
        float xoffset = xpos - lastMouseX;
        float yoffset = lastMouseY - ypos;  // Reversed since y-coordinates go from bottom to top
        
        camera.rotate(xoffset, yoffset);
    }
    
    if (rightMousePressed) {
        float xoffset = xpos - lastMouseX;
        float yoffset = lastMouseY - ypos;
        
        camera.pan(-xoffset, yoffset);
    }
    
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.zoom(-yoffset * 5.0f);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}