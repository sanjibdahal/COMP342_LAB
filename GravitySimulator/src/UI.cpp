#include "UI.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

UI::UI(GLFWwindow* window)
    : window(window), selectedMode(SimulationMode::SOLAR_SYSTEM),
      newModeSelected(false), showMenu(true), showHelp(true), showStats(true) {
}

void UI::render(const std::vector<std::unique_ptr<Body>>& bodies,
                float deltaTime, float timeScale, bool paused) {
    displayStats(1.0f / deltaTime, bodies.size());
    
    if (showMenu) {
        displayMenu();
    }
    
    if (showHelp) {
        displayControls();
    }
    
    displayInfoPanel(bodies);
}

void UI::displayMenu() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Simple text menu in top-left
    float y = height - 30.0f;
    
    renderText("=== SIMULATION MODES ===", 10, y, glm::vec3(1.0f, 1.0f, 0.0f));
    y -= 20;
    renderText("1: Solar System", 10, y, glm::vec3(1.0f));
    y -= 20;
    renderText("2: Three-Body Problem", 10, y, glm::vec3(1.0f));
    y -= 20;
    renderText("3: Binary Star System", 10, y, glm::vec3(1.0f));
    y -= 20;
    renderText("4: Figure-8 Orbit", 10, y, glm::vec3(1.0f));
    y -= 20;
    renderText("5: Galaxy Collision", 10, y, glm::vec3(1.0f));
    y -= 20;
    renderText("6: Sandbox Mode", 10, y, glm::vec3(1.0f));
}

void UI::displayControls() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    float y = height / 2.0f;
    
    renderText("=== CONTROLS ===", width - 250, y, glm::vec3(0.0f, 1.0f, 1.0f));
    y -= 20;
    renderText("SPACE: Pause/Resume", width - 250, y, glm::vec3(1.0f));
    y -= 20;
    renderText("R: Reset", width - 250, y, glm::vec3(1.0f));
    y -= 20;
    renderText("T: Toggle Trails", width - 250, y, glm::vec3(1.0f));
    y -= 20;
    renderText("V: Velocity Vectors", width - 250, y, glm::vec3(1.0f));
    y -= 20;
    renderText("F: Force Vectors", width - 250, y, glm::vec3(1.0f));
    y -= 20;
    renderText("+/-: Time Speed", width - 250, y, glm::vec3(1.0f));
    y -= 20;
    renderText("Mouse: Camera Control", width - 250, y, glm::vec3(1.0f));
}

void UI::displayStats(float fps, int bodyCount) {
    std::stringstream ss;
    ss << "FPS: " << std::fixed << std::setprecision(1) << fps;
    renderText(ss.str(), 10, 30, glm::vec3(0.0f, 1.0f, 0.0f));
    
    ss.str("");
    ss << "Bodies: " << bodyCount;
    renderText(ss.str(), 10, 50, glm::vec3(1.0f, 1.0f, 0.0f));
}

void UI::displayInfoPanel(const std::vector<std::unique_ptr<Body>>& bodies) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    float y = 100;
    
    for (const auto& body : bodies) {
        if (body->isSelected) {
            renderText("Selected: " + body->name, 10, y, glm::vec3(1.0f, 1.0f, 0.0f));
            y += 20;
            
            std::stringstream ss;
            ss << "Mass: " << std::fixed << std::setprecision(2) << body->mass;
            renderText(ss.str(), 10, y, glm::vec3(1.0f));
            y += 20;
            
            ss.str("");
            ss << "Pos: (" << body->position.x << ", " << body->position.y 
               << ", " << body->position.z << ")";
            renderText(ss.str(), 10, y, glm::vec3(1.0f));
            y += 20;
            
            float speed = glm::length(body->velocity);
            ss.str("");
            ss << "Speed: " << speed;
            renderText(ss.str(), 10, y, glm::vec3(1.0f));
            break;
        }
    }
}

void UI::displayEnergyGraph(const std::vector<std::unique_ptr<Body>>& bodies) {
    // Simple energy conservation visualization
    // This would show a graph of total energy over time
}

void UI::renderText(const std::string& text, float x, float y, const glm::vec3& color) {
    // Using GLFW's built-in bitmap font (simple but functional)
    // For production, use FreeType or a texture atlas
    
    // This is a placeholder - actual implementation would use proper text rendering
    // For now, just print to console for debugging
    static int frameCount = 0;
    if (frameCount++ % 60 == 0) {
        // std::cout << text << std::endl;
    }
}

void UI::renderBox(float x, float y, float width, float height, const glm::vec4& color) {
    // Simple 2D quad rendering for UI boxes
    glDisable(GL_DEPTH_TEST);
    
    // Setup orthographic projection for 2D rendering
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