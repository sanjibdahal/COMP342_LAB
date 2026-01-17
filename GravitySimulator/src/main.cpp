#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <sstream>
#include <iomanip>

#include "Body.h"
#include "PhysicsEngine.h"
#include "Camera.h"
#include "Renderer.h"
#include "UI.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global state
Camera camera(glm::vec3(0.0f, 80.0f, 180.0f), glm::vec3(0.0f));
Renderer renderer;
PhysicsEngine physics(1.0e10f);
std::vector<std::unique_ptr<Body>> bodies;

bool paused = false;
float timeScale = 1.0f;
bool leftMousePressed = false;
bool rightMousePressed = false;
bool middleMousePressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

std::string currentSimulationName = "Solar System";
std::string currentSimulationDescription = "Planetary system with central star";

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
void displaySimulationInfo();
void selectBodyUnderCursor(GLFWwindow* window, double xpos, double ypos);
void displayWelcome();

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                         "Gravity Simulator - N-Body Physics with Einstein Spacetime Warp", 
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
    
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    glLineWidth(1.5f);
    
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    setupSimulation(UI::SimulationMode::SOLAR_SYSTEM);
    UI ui(window);
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int frameCount = 0;
    
    displayWelcome();
    displaySimulationInfo();
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameCount++;
        
        glfwPollEvents();
        
        if (!paused) {
            physics.update(bodies, deltaTime * timeScale);
        }
        
        camera.update(deltaTime);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspectRatio = (float)width / (float)height;
        
        renderer.render(bodies, camera, aspectRatio);
        ui.render(bodies, deltaTime, timeScale, paused);
        
        // Print system info periodically
        if (frameCount % 300 == 0) {
            ui.printSystemInfo(bodies, physics);
        }
        
        if (ui.hasNewModeSelected()) {
            setupSimulation(ui.getSelectedMode());
            ui.resetModeSelection();
        }
        
        glfwSwapBuffers(window);
    }
    
    renderer.cleanup();
    glfwTerminate();
    return 0;
}

void displayWelcome() {
    std::cout << "\n\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║        GRAVITY SIMULATOR - N-BODY PHYSICS ENGINE                 ║\n";
    std::cout << "║              with Einstein Spacetime Curvature                   ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Kathmandu University - Department of Computer Engineering       ║\n";
    std::cout << "║  Developers: Sanjib Dahal & Aryaman Giri                        ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "🎮 KEYBOARD CONTROLS:\n";
    std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ SIMULATION MODES:                                            │\n";
    std::cout << "│  1  →  Solar System          4  →  Figure-8 Orbit           │\n";
    std::cout << "│  2  →  Three-Body Problem    5  →  Galaxy Collision         │\n";
    std::cout << "│  3  →  Binary Star System    6  →  Sandbox Mode             │\n";
    std::cout << "├──────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ SIMULATION CONTROL:                                          │\n";
    std::cout << "│  SPACE  →  Pause/Resume      R  →  Reset Simulation         │\n";
    std::cout << "│  +/-    →  Time Scale        C  →  Clear Trails             │\n";
    std::cout << "├──────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ VISUALIZATION:                                               │\n";
    std::cout << "│  T  →  Toggle Trails         W  →  Toggle Spacetime Warp    │\n";
    std::cout << "│  V  →  Toggle Velocity       A  →  Toggle Axes              │\n";
    std::cout << "│  F  →  Toggle Forces         I  →  Toggle Info Display      │\n";
    std::cout << "│  [  →  Decrease Warp         ]  →  Increase Warp            │\n";
    std::cout << "├──────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ SANDBOX MODE:                                                │\n";
    std::cout << "│  N  →  Add Random Body       DELETE  →  Remove Selected     │\n";
    std::cout << "│  LEFT CLICK  →  Select Body                                  │\n";
    std::cout << "└──────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "🖱️  MOUSE CONTROLS:\n";
    std::cout << "  • Left Click + Drag    →  Rotate Camera\n";
    std::cout << "  • Right Click + Drag   →  Pan Camera\n";
    std::cout << "  • Scroll Wheel         →  Zoom In/Out\n";
    std::cout << "  • Left Click on Body   →  Select/View Info\n\n";
    
    std::cout << "📊 FEATURES:\n";
    std::cout << "  ✓ Real-time N-body gravitational simulation\n";
    std::cout << "  ✓ Einstein spacetime curvature visualization\n";
    std::cout << "  ✓ RK4 numerical integration for accuracy\n";
    std::cout << "  ✓ Energy & momentum conservation display\n";
    std::cout << "  ✓ Interactive body selection and information\n";
    std::cout << "  ✓ Force and velocity vector visualization\n";
    std::cout << "  ✓ Collision detection and response\n\n";
    
    std::cout << "═══════════════════════════════════════════════════════════════\n\n";
}

void setupSimulation(UI::SimulationMode mode) {
    bodies.clear();
    
    switch (mode) {
        case UI::SimulationMode::SOLAR_SYSTEM:
            setupSolarSystem();
            currentSimulationName = "Solar System";
            currentSimulationDescription = "Inner planets orbiting the Sun";
            std::cout << "\n🌟 SIMULATION: Solar System\n";
            std::cout << "   Realistic planetary orbits with stable central star\n";
            break;
            
        case UI::SimulationMode::THREE_BODY:
            setupThreeBodyProblem();
            currentSimulationName = "Three-Body Problem";
            currentSimulationDescription = "Chaotic three-body gravitational dance";
            std::cout << "\n⚛️  SIMULATION: Three-Body Problem\n";
            std::cout << "   Chaotic system with unpredictable long-term behavior\n";
            break;
            
        case UI::SimulationMode::BINARY_STAR:
            setupBinaryStarSystem();
            currentSimulationName = "Binary Star System";
            currentSimulationDescription = "Two stars with orbiting planets";
            std::cout << "\n✨ SIMULATION: Binary Star System\n";
            std::cout << "   Twin stars with complex planetary dynamics\n";
            break;
            
        case UI::SimulationMode::FIGURE_8:
            setupFigure8Orbit();
            currentSimulationName = "Figure-8 Orbit";
            currentSimulationDescription = "Choreographic periodic solution";
            std::cout << "\n∞  SIMULATION: Figure-8 Orbit\n";
            std::cout << "   Stable periodic three-body choreography\n";
            break;
            
        case UI::SimulationMode::GALAXY_COLLISION:
            setupGalaxyCollision();
            currentSimulationName = "Galaxy Collision";
            currentSimulationDescription = "Two galaxies merging";
            std::cout << "\n🌌 SIMULATION: Galaxy Collision\n";
            std::cout << "   Galactic merger with tidal interactions\n";
            break;
            
        case UI::SimulationMode::SANDBOX:
            setupSandboxMode();
            currentSimulationName = "Sandbox Mode";
            currentSimulationDescription = "Interactive creation mode";
            std::cout << "\n🎨 SIMULATION: Sandbox Mode\n";
            std::cout << "   Press 'N' to add bodies, Click to select\n";
            break;
    }
    
    displaySimulationInfo();
}

void displaySimulationInfo() {
    std::cout << "─────────────────────────────────────────────────────────\n";
    std::cout << "📊 Active: " << currentSimulationName << "\n";
    std::cout << "   " << currentSimulationDescription << "\n";
    std::cout << "   Bodies: " << bodies.size() 
              << " | Time Scale: " << std::fixed << std::setprecision(1) 
              << timeScale << "x"
              << " | " << (paused ? "[PAUSED]" : "[RUNNING]") << "\n";
    std::cout << "─────────────────────────────────────────────────────────\n\n";
}

void setupSolarSystem() {
    // Sun
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        5000.0f, 5.0f,
        glm::vec3(1.0f, 0.9f, 0.2f),
        "Sun", true
    ));
    
    // Mercury
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(30.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 18.0f),
        10.0f, 1.0f,
        glm::vec3(0.7f, 0.7f, 0.7f),
        "Mercury"
    ));
    
    // Venus
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(45.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 15.0f),
        20.0f, 1.5f,
        glm::vec3(0.9f, 0.7f, 0.4f),
        "Venus"
    ));
    
    // Earth
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(60.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 13.0f),
        25.0f, 1.8f,
        glm::vec3(0.2f, 0.4f, 0.8f),
        "Earth"
    ));
    
    // Mars
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(80.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 11.0f),
        15.0f, 1.2f,
        glm::vec3(0.8f, 0.3f, 0.2f),
        "Mars"
    ));
    
    // Jupiter
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(120.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 8.5f),
        100.0f, 3.5f,
        glm::vec3(0.8f, 0.7f, 0.6f),
        "Jupiter"
    ));
}

void setupThreeBodyProblem() {
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-30.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 8.0f),
        100.0f, 3.0f,
        glm::vec3(1.0f, 0.2f, 0.2f),
        "Body 1"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(30.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -8.0f),
        100.0f, 3.0f,
        glm::vec3(0.2f, 1.0f, 0.2f),
        "Body 2"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 40.0f),
        glm::vec3(-6.0f, 0.0f, 0.0f),
        100.0f, 3.0f,
        glm::vec3(0.2f, 0.2f, 1.0f),
        "Body 3"
    ));
}

void setupBinaryStarSystem() {
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-40.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 6.0f),
        2000.0f, 4.0f,
        glm::vec3(0.3f, 0.5f, 1.0f),
        "Star A"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(40.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -6.0f),
        2000.0f, 4.0f,
        glm::vec3(1.0f, 0.3f, 0.3f),
        "Star B"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-60.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 12.0f),
        20.0f, 1.5f,
        glm::vec3(0.5f, 0.8f, 0.5f),
        "Planet A1"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(60.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -12.0f),
        20.0f, 1.5f,
        glm::vec3(0.8f, 0.5f, 0.8f),
        "Planet B1"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 100.0f),
        glm::vec3(7.0f, 0.0f, 0.0f),
        15.0f, 1.2f,
        glm::vec3(0.7f, 0.7f, 0.3f),
        "Circumbinary"
    ));
}

void setupFigure8Orbit() {
    float m = 100.0f;
    float v = 5.5f;
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-25.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, v),
        m, 3.0f,
        glm::vec3(1.0f, 0.3f, 0.3f),
        "Body 1"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(25.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, v),
        m, 3.0f,
        glm::vec3(0.3f, 1.0f, 0.3f),
        "Body 2"
    ));
    
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -2.0f * v),
        m, 3.0f,
        glm::vec3(0.3f, 0.3f, 1.0f),
        "Body 3"
    ));
}

void setupGalaxyCollision() {
    // Galaxy 1
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(-80.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 3.0f),
        3000.0f, 5.0f,
        glm::vec3(0.9f, 0.8f, 0.3f),
        "Galaxy 1 Core"
    ));
    
    for (int i = 0; i < 8; i++) {
        float angle = i * 2.0f * M_PI / 8.0f;
        float radius = 20.0f + (i % 3) * 10.0f;
        float speed = 5.0f + (i % 3) * 2.0f;
        
        bodies.push_back(std::make_unique<Body>(
            glm::vec3(-80.0f + radius * cos(angle), 0.0f, radius * sin(angle)),
            glm::vec3(-speed * sin(angle), 0.0f, 3.0f + speed * cos(angle)),
            15.0f, 1.0f,
            glm::vec3(0.8f, 0.8f, 0.9f),
            "G1-Star" + std::to_string(i)
        ));
    }
    
    // Galaxy 2
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(80.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -3.0f),
        3000.0f, 5.0f,
        glm::vec3(0.3f, 0.6f, 0.9f),
        "Galaxy 2 Core"
    ));
    
    for (int i = 0; i < 8; i++) {
        float angle = i * 2.0f * M_PI / 8.0f;
        float radius = 20.0f + (i % 3) * 10.0f;
        float speed = 5.0f + (i % 3) * 2.0f;
        
        bodies.push_back(std::make_unique<Body>(
            glm::vec3(80.0f + radius * cos(angle), 0.0f, radius * sin(angle)),
            glm::vec3(-speed * sin(angle), 0.0f, -3.0f + speed * cos(angle)),
            15.0f, 1.0f,
            glm::vec3(0.9f, 0.9f, 1.0f),
            "G2-Star" + std::to_string(i)
        ));
    }
}

void setupSandboxMode() {
    bodies.push_back(std::make_unique<Body>(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        1000.0f, 4.0f,
        glm::vec3(1.0f, 1.0f, 0.5f),
        "Central Body", true
    ));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
                
            case GLFW_KEY_SPACE:
                paused = !paused;
                std::cout << (paused ? "⏸️  Paused" : "▶️  Resumed") << std::endl;
                break;
                
            case GLFW_KEY_R:
                std::cout << "🔄 Resetting simulation..." << std::endl;
                for (auto& body : bodies) {
                    body->clearTrail();
                }
                break;
                
            case GLFW_KEY_T:
                renderer.toggleTrails();
                std::cout << "🛤️  Trails: " << (renderer.isShowingTrails() ? "ON" : "OFF") << std::endl;
                break;
                
            case GLFW_KEY_V:
                renderer.toggleVelocityVectors();
                std::cout << "➡️  Velocity vectors: " << (renderer.isShowingVelocity() ? "ON" : "OFF") << std::endl;
                break;
                
            case GLFW_KEY_F:
                renderer.toggleForceVectors();
                std::cout << "⚡ Force vectors: " << (renderer.isShowingForces() ? "ON" : "OFF") << std::endl;
                break;
                
            case GLFW_KEY_G:
                renderer.toggleGrid();
                std::cout << "📐 Grid toggled" << std::endl;
                break;
                
            case GLFW_KEY_A:
                renderer.toggleAxes();
                std::cout << "🎯 Axes toggled" << std::endl;
                break;
                
            case GLFW_KEY_EQUAL:
            case GLFW_KEY_KP_ADD:
                timeScale *= 1.5f;
                std::cout << "⏩ Time scale: " << timeScale << "x" << std::endl;
                break;
                
            case GLFW_KEY_MINUS:
            case GLFW_KEY_KP_SUBTRACT:
                timeScale /= 1.5f;
                if (timeScale < 0.1f) timeScale = 0.1f;
                std::cout << "⏪ Time scale: " << timeScale << "x" << std::endl;
                break;
                
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
                    std::cout << "➕ Added new body. Total: " << bodies.size() << std::endl;
                }
                break;
                
            case GLFW_KEY_DELETE:
            case GLFW_KEY_BACKSPACE:
                for (auto it = bodies.begin(); it != bodies.end(); ++it) {
                    if ((*it)->isSelected && !(*it)->isFixed) {
                        std::cout << "➖ Removed " << (*it)->name << std::endl;
                        bodies.erase(it);
                        break;
                    }
                }
                break;
                
            case GLFW_KEY_C:
                for (auto& body : bodies) {
                    body->clearTrail();
                }
                std::cout << "🧹 Cleared all trails" << std::endl;
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
        float yoffset = lastMouseY - ypos;
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