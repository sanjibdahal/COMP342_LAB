#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "Body.h"
#include <vector>
#include <memory>

class PhysicsEngine {
public:
    static constexpr float G = 6.674e-11f;
    static constexpr float SOFTENING = 0.1f;
    
    PhysicsEngine(float gravityScale = 1.0e10f);
    
    void update(std::vector<std::unique_ptr<Body>>& bodies, float deltaTime);
    void computeForces(std::vector<std::unique_ptr<Body>>& bodies);
    void detectAndResolveCollisions(std::vector<std::unique_ptr<Body>>& bodies);
    
    void eulerIntegration(std::vector<std::unique_ptr<Body>>& bodies, float dt);
    void rk4Integration(std::vector<std::unique_ptr<Body>>& bodies, float dt);
    
    glm::vec3 calculateGravitationalForce(const Body& b1, const Body& b2) const;
    
    bool checkCollision(const Body& b1, const Body& b2) const;
    void resolveCollision(Body& b1, Body& b2);
    
    float getTotalEnergy(const std::vector<std::unique_ptr<Body>>& bodies) const;
    glm::vec3 getTotalMomentum(const std::vector<std::unique_ptr<Body>>& bodies) const;
    glm::vec3 getCenterOfMass() const;
    
    // Setters
    void setGravityScale(float scale) { gravityScale = scale; }
    void setUseRK4(bool use) { useRK4 = use; }
    void setCollisionsEnabled(bool enabled) { collisionsEnabled = enabled; }
    void setDampingEnabled(bool enabled) { dampingEnabled = enabled; }
    void setCenteringForceEnabled(bool enabled) { centeringForceEnabled = enabled; }
    void setMaxVelocity(float maxVel) { maxVelocity = maxVel; }
    
    // Getters
    float getGravityScale() const { return gravityScale; }
    bool isUsingRK4() const { return useRK4; }
    bool isDampingEnabled() const { return dampingEnabled; }
    
private:
    float gravityScale;
    bool useRK4;
    bool collisionsEnabled;
    bool dampingEnabled;
    bool centeringForceEnabled;
    float dampingFactor;
    float maxVelocity;
    glm::vec3 centerOfMass;
    
    struct Derivative {
        glm::vec3 velocity;
        glm::vec3 acceleration;
    };
    
    struct State {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 force;
    };
    
    void updateCenterOfMass(const std::vector<std::unique_ptr<Body>>& bodies);
    void applyStabilityConstraints(std::vector<std::unique_ptr<Body>>& bodies);
    Derivative evaluateDerivative(Body& body, float dt, const Derivative& d);
};

#endif