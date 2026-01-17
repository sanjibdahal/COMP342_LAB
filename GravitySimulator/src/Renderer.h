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
    Renderer();
    ~Renderer();
    
    bool initialize();
    void cleanup();
    
    void render(const std::vector<std::unique_ptr<Body>>& bodies, 
                const Camera& camera, 
                float aspectRatio);
    
    void renderBodies(const std::vector<std::unique_ptr<Body>>& bodies,
                     const glm::mat4& view, const glm::mat4& projection,
                     const glm::vec3& cameraPos);
    void renderTrails(const std::vector<std::unique_ptr<Body>>& bodies,
                     const glm::mat4& view, const glm::mat4& projection);
    void renderVectors(const std::vector<std::unique_ptr<Body>>& bodies,
                      const glm::mat4& view, const glm::mat4& projection);
    void renderAxes(const glm::mat4& view, const glm::mat4& projection);
    void renderSpacetimeWarp(const std::vector<std::unique_ptr<Body>>& bodies,
                            const glm::mat4& view, const glm::mat4& projection);
    void renderDirectionIndicators(const std::vector<std::unique_ptr<Body>>& bodies,
                                   const glm::mat4& view, const glm::mat4& projection);
    
    // Toggle options
    void toggleTrails() { showTrails = !showTrails; }
    void toggleVelocityVectors() { showVelocity = !showVelocity; }
    void toggleForceVectors() { showForces = !showForces; }
    void toggleAxes() { showAxes = !showAxes; }
    void toggleSpacetimeWarp() { showSpacetimeWarp = !showSpacetimeWarp; }
    void toggleGrid() { showGrid = !showGrid; }
    void toggleInfo() { showInfo = !showInfo; }
    
    // Getters
    bool isShowingTrails() const { return showTrails; }
    bool isShowingVelocity() const { return showVelocity; }
    bool isShowingForces() const { return showForces; }
    bool isShowingSpacetimeWarp() const { return showSpacetimeWarp; }
    
    // Setters
    void setWarpIntensity(float intensity) { warpIntensity = intensity; }
    void setSelectedBody(int index) { selectedBodyIndex = index; }
    
private:
    Shader bodyShader;
    Shader lineShader;
    Shader gridShader;
    
    GLuint sphereVAO, sphereVBO, sphereEBO;
    unsigned int sphereIndexCount;
    
    GLuint lineVAO, lineVBO;
    
    GLuint gridVAO, gridVBO;
    
    GLuint spacetimeVAO, spacetimeVBO, spacetimeEBO;
    unsigned int spacetimeIndexCount;
    
    GLuint axesVAO, axesVBO;
    
    bool showTrails;
    bool showVelocity;
    bool showForces;
    bool showGrid;
    bool showAxes;
    bool showSpacetimeWarp;
    bool showInfo;
    
    float warpIntensity;
    int selectedBodyIndex;
    
    void createSphere(float radius, unsigned int rings, unsigned int sectors);
    void createGrid(float size, int divisions);
    void renderGrid(const glm::mat4& view, const glm::mat4& projection);
    void createSpacetimeGrid(float size, int divisions);
    void createAxes();
    void updateSpacetimeWarp(const std::vector<std::unique_ptr<Body>>& bodies);
    void renderSphere(const glm::vec3& position, float radius, const glm::vec3& color,
                     const glm::mat4& view, const glm::mat4& projection);
    void renderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color,
                   const glm::mat4& view, const glm::mat4& projection);
    void renderArrow(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color,
                    const glm::mat4& view, const glm::mat4& projection);
    void renderOrbitCircle(float radius, const glm::vec3& color,
                          const glm::mat4& view, const glm::mat4& projection);
};

#endif