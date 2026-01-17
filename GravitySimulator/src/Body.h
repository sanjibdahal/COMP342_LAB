#ifndef BODY_H
#define BODY_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Body {
public:
    // Physical properties
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 force;
    float mass;
    float radius;
    
    // Visual properties
    glm::vec3 color;
    std::string name;
    bool isFixed;  // For stars/sun
    
    // Trail rendering
    std::vector<glm::vec3> trail;
    int maxTrailLength;
    
    // Selection and UI
    bool isSelected;
    
    // Constructor
    Body(const glm::vec3& pos = glm::vec3(0.0f),
         const glm::vec3& vel = glm::vec3(0.0f),
         float m = 1.0f,
         float r = 1.0f,
         const glm::vec3& col = glm::vec3(1.0f),
         const std::string& n = "Body",
         bool fixed = false);
    
    // Physics methods
    void applyForce(const glm::vec3& f);
    void update(float deltaTime);
    void resetForces();
    
    // Trail management
    void updateTrail();
    void clearTrail();
    
    // Utility
    float getKineticEnergy() const;
    float getPotentialEnergy(const std::vector<Body*>& others, float G) const;
    glm::vec3 getMomentum() const;
};

#endif