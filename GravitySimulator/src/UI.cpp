#include "UI.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

UI::UI(GLFWwindow* window)
    : window(window), selectedMode(SimulationMode::SOLAR_SYSTEM),
      newModeSelected(false), showMenu(true), showHelp(true), 
      showStats(true), showBodyInfo(true) {
}

void UI::render(const std::vector<std::unique_ptr<Body>>& bodies,
                float deltaTime, float timeScale, bool paused) {
    displayStats(1.0f / deltaTime, bodies.size(), timeScale, paused);
    
    if (showMenu) {
        displayMenu();
    }
    
    if (showHelp) {
        displayControls();
    }
    
    if (showBodyInfo) {
        displayDetailedBodyInfo(bodies);
    }
}

void UI::displayMenu() {
    // Display simulation mode menu
    std::cout << "\r"; // Carriage return for same-line updates
}

void UI::displayControls() {
    // Controls are displayed at startup
}

void UI::displayStats(float fps, int bodyCount, float timeScale, bool paused) {
    // Console output for stats
    static int frameCounter = 0;
    if (frameCounter++ % 60 == 0) { // Update every 60 frames
        std::cout << "\r"; // Keep on same line
        std::cout << "FPS: " << std::fixed << std::setprecision(1) << fps 
                  << " | Bodies: " << bodyCount 
                  << " | Time: " << timeScale << "x"
                  << " | " << (paused ? "[PAUSED]" : "[RUNNING]")
                  << "          " << std::flush;
    }
}

void UI::displayDetailedBodyInfo(const std::vector<std::unique_ptr<Body>>& bodies) {
    static int updateCounter = 0;
    if (updateCounter++ % 120 != 0) return; // Update every 2 seconds
    
    // Find most massive body
    const Body* mostMassive = nullptr;
    float maxMass = 0.0f;
    
    for (const auto& body : bodies) {
        if (body->mass > maxMass) {
            maxMass = body->mass;
            mostMassive = body.get();
        }
    }
    
    // Calculate system properties
    float totalKE = 0.0f;
    glm::vec3 totalMomentum(0.0f);
    
    for (const auto& body : bodies) {
        totalKE += body->getKineticEnergy();
        totalMomentum += body->getMomentum();
    }
}

void UI::displayInfoPanel(const std::vector<std::unique_ptr<Body>>& bodies) {
    for (const auto& body : bodies) {
        if (body->isSelected) {
            displayBodyProperties(*body);
            break;
        }
    }
}

void UI::displayBodyProperties(const Body& body) {
    std::cout << "\n╔════════════════════ BODY INFORMATION ═══════════════════╗\n";
    std::cout << "║ Name: " << std::left << std::setw(46) << body.name << "║\n";
    std::cout << "╠═════════════════════════════════════════════════════════╣\n";
    
    std::cout << "║ Mass:     " << std::fixed << std::setprecision(2) 
              << std::setw(44) << body.mass << "║\n";
    
    std::cout << "║ Position: (" 
              << std::setprecision(1) << std::setw(6) << body.position.x << ", "
              << std::setw(6) << body.position.y << ", "
              << std::setw(6) << body.position.z << ")           ║\n";
    
    float speed = glm::length(body.velocity);
    std::cout << "║ Velocity: (" 
              << std::setprecision(1) << std::setw(6) << body.velocity.x << ", "
              << std::setw(6) << body.velocity.y << ", "
              << std::setw(6) << body.velocity.z << ")           ║\n";
    
    std::cout << "║ Speed:    " << std::setw(44) << speed << "║\n";
    
    float forceMag = glm::length(body.force);
    std::cout << "║ Force:    " << std::setw(44) << forceMag << "║\n";
    
    std::cout << "║ Kinetic Energy: " << std::setw(38) 
              << body.getKineticEnergy() << "║\n";
    
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";
}

void UI::displayEnergyGraph(const std::vector<std::unique_ptr<Body>>& bodies) {
    // Energy conservation tracking
}

void UI::renderText(const std::string& text, float x, float y, const glm::vec3& color) {
    // Placeholder for text rendering
}

void UI::renderBox(float x, float y, float width, float height, const glm::vec4& color) {
    glDisable(GL_DEPTH_TEST);
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
}

void UI::renderProgressBar(float x, float y, float width, float height,
                          float value, const glm::vec3& color) {
    renderBox(x, y, width, height, glm::vec4(0.2f, 0.2f, 0.2f, 0.8f));
    renderBox(x + 2, y + 2, (width - 4) * value, height - 4, 
              glm::vec4(color, 0.8f));
}

void UI::printSystemInfo(const std::vector<std::unique_ptr<Body>>& bodies, 
                        const PhysicsEngine& physics) {
    static int counter = 0;
    if (counter++ % 300 != 0) return; // Every 5 seconds
    
    std::cout << "\n┌─────────────── SYSTEM STATUS ───────────────┐\n";
    
    float totalEnergy = physics.getTotalEnergy(bodies);
    glm::vec3 totalMomentum = physics.getTotalMomentum(bodies);
    glm::vec3 centerOfMass = physics.getCenterOfMass();
    
    std::cout << "│ Total Energy:    " << std::fixed << std::setprecision(2) 
              << std::setw(22) << totalEnergy << " │\n";
    
    float momentumMag = glm::length(totalMomentum);
    std::cout << "│ Total Momentum:  " << std::setw(22) << momentumMag << " │\n";
    
    std::cout << "│ Center of Mass:  (" 
              << std::setprecision(1) << centerOfMass.x << ", "
              << centerOfMass.y << ", " << centerOfMass.z << ")    │\n";
    
    std::cout << "└─────────────────────────────────────────────┘\n\n";
}

void UI::toggleBodyInfo() {
    showBodyInfo = !showBodyInfo;
}