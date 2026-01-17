#ifndef UI_H
#define UI_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Body.h"
#include "PhysicsEngine.h"
#include <vector>
#include <memory>
#include <string>

class UI {
public:
    enum class SimulationMode {
        SOLAR_SYSTEM,
        THREE_BODY,
        BINARY_STAR,
        FIGURE_8,
        GALAXY_COLLISION,
        SANDBOX
    };
    
    UI(GLFWwindow* window);
    
    void render(const std::vector<std::unique_ptr<Body>>& bodies,
               float deltaTime, float timeScale, bool paused);
    
    void displayInfoPanel(const std::vector<std::unique_ptr<Body>>& bodies);
    void displayControls();
    void displayStats(float fps, int bodyCount, float timeScale, bool paused);
    void displayEnergyGraph(const std::vector<std::unique_ptr<Body>>& bodies);
    void displayDetailedBodyInfo(const std::vector<std::unique_ptr<Body>>& bodies);
    void displayBodyProperties(const Body& body);
    
    void displayMenu();
    SimulationMode getSelectedMode() const { return selectedMode; }
    bool hasNewModeSelected() const { return newModeSelected; }
    void resetModeSelection() { newModeSelected = false; }
    
    void renderText(const std::string& text, float x, float y, const glm::vec3& color);
    
    void printSystemInfo(const std::vector<std::unique_ptr<Body>>& bodies,
                        const PhysicsEngine& physics);
    
    void toggleBodyInfo();
    
private:
    GLFWwindow* window;
    SimulationMode selectedMode;
    bool newModeSelected;
    bool showMenu;
    bool showHelp;
    bool showStats;
    bool showBodyInfo;
    
    std::vector<float> energyHistory;
    static constexpr int MAX_HISTORY = 200;
    
    void renderBox(float x, float y, float width, float height, const glm::vec4& color);
    void renderProgressBar(float x, float y, float width, float height, 
                          float value, const glm::vec3& color);
};

#endif