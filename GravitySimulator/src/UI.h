#ifndef UI_H
#define UI_H

// CRITICAL: GLEW must come BEFORE GLFW!
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Body.h"
#include <vector>
#include <memory>
#include <string>

class UI {
public:
    // Simulation modes
    enum class SimulationMode {
        SOLAR_SYSTEM,
        THREE_BODY,
        BINARY_STAR,
        FIGURE_8,
        GALAXY_COLLISION,
        SANDBOX
    };
    
    // Constructor
    UI(GLFWwindow* window);
    
    // Render UI elements
    void render(const std::vector<std::unique_ptr<Body>>& bodies,
               float deltaTime, float timeScale, bool paused);
    
    // Display information panels
    void displayInfoPanel(const std::vector<std::unique_ptr<Body>>& bodies);
    void displayControls();
    void displayStats(float fps, int bodyCount);
    void displayEnergyGraph(const std::vector<std::unique_ptr<Body>>& bodies);
    
    // Menu system
    void displayMenu();
    SimulationMode getSelectedMode() const { return selectedMode; }
    bool hasNewModeSelected() const { return newModeSelected; }
    void resetModeSelection() { newModeSelected = false; }
    
    // Text rendering (simple bitmap text)
    void renderText(const std::string& text, float x, float y, const glm::vec3& color);
    
private:
    GLFWwindow* window;
    SimulationMode selectedMode;
    bool newModeSelected;
    bool showMenu;
    bool showHelp;
    bool showStats;
    
    // Energy history for graphing
    std::vector<float> energyHistory;
    static constexpr int MAX_HISTORY = 200;
    
    // Helper methods
    void renderBox(float x, float y, float width, float height, const glm::vec4& color);
    void renderProgressBar(float x, float y, float width, float height, 
                          float value, const glm::vec3& color);
};

#endif