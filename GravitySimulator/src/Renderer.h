#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "Shader.h"
#include "Body.h"
#include "Camera.h"
#include <vector>
#include <memory>

class Renderer {
public:
    // Constructor/Destructor
    Renderer();
    ~Renderer();
    
    // Initialization
    bool initialize();
    void cleanup();
    
    // Main rendering
    void render(const std::vector<std::unique_ptr<Body>>& bodies, 
                const Camera& camera, 
                float aspectRatio);
    
    // Render components
    void renderBodies(const std::vector<std::unique_ptr<Body>>& bodies,
                     const glm::mat4& view, const glm::mat4& projection);
    void renderTrails(const std::vector<std::unique_ptr<Body>>& bodies,
                     const glm::mat4& view, const glm::mat4& projection);
    void renderVectors(const std::vector<std::unique_ptr<Body>>& bodies,
                      const glm::mat4& view, const glm::mat4& projection);
    void renderGrid(const glm::mat4& view, const glm::mat4& projection);
    void renderAxes(const glm::mat4& view, const glm::mat4& projection);
    
    // Toggle options
    void toggleTrails() { showTrails = !showTrails; }
    void toggleVelocityVectors() { showVelocity = !showVelocity; }
    void toggleForceVectors() { showForces = !showForces; }
    void toggleGrid() { showGrid = !showGrid; }
    void toggleAxes() { showAxes = !showAxes; }
    
    // Getters
    bool isShowingTrails() const { return showTrails; }
    bool isShowingVelocity() const { return showVelocity; }
    bool isShowingForces() const { return showForces; }
    
private:
    // Shaders
    Shader bodyShader;
    Shader lineShader;
    
    // Sphere mesh data
    GLuint sphereVAO, sphereVBO, sphereEBO;
    unsigned int sphereIndexCount;
    
    // Line rendering
    GLuint lineVAO, lineVBO;
    
    // Grid and axes
    GLuint gridVAO, gridVBO;
    GLuint axesVAO, axesVBO;
    
    // Rendering flags
    bool showTrails;
    bool showVelocity;
    bool showForces;
    bool showGrid;
    bool showAxes;
    
    // Helper methods
    void createSphere(float radius, unsigned int rings, unsigned int sectors);
    void createGrid(float size, int divisions);
    void createAxes();
    void renderSphere(const glm::vec3& position, float radius, const glm::vec3& color,
                     const glm::mat4& view, const glm::mat4& projection);
    void renderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color,
                   const glm::mat4& view, const glm::mat4& projection);
};

#endif