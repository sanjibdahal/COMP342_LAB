#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "Body.h"
#include <vector>
#include <memory>

class PhysicsEngine {
public:
    // Constants
    static constexpr float G = 6.674e-11f;  // Gravitational constant (scaled)
    static constexpr float SOFTENING = 0.1f;  // Softening parameter to prevent singularities
    
    // Constructor
    PhysicsEngine(float gravityScale = 1.0e10f);
    
    // Main simulation methods
    void update(std::vector<std::unique_ptr<Body>>& bodies, float deltaTime);
    void computeForces(std::vector<std::unique_ptr<Body>>& bodies);
    void detectAndResolveCollisions(std::vector<std::unique_ptr<Body>>& bodies);
    
    // Integration methods
    void eulerIntegration(std::vector<std::unique_ptr<Body>>& bodies, float dt);
    void rk4Integration(std::vector<std::unique_ptr<Body>>& bodies, float dt);
    
    // Gravity calculation
    glm::vec3 calculateGravitationalForce(const Body& b1, const Body& b2) const;
    
    // Collision handling
    bool checkCollision(const Body& b1, const Body& b2) const;
    void resolveCollision(Body& b1, Body& b2);
    
    // Energy and momentum
    float getTotalEnergy(const std::vector<std::unique_ptr<Body>>& bodies) const;
    glm::vec3 getTotalMomentum(const std::vector<std::unique_ptr<Body>>& bodies) const;
    
    // Setters
    void setGravityScale(float scale) { gravityScale = scale; }
    void setUseRK4(bool use) { useRK4 = use; }
    void setCollisionsEnabled(bool enabled) { collisionsEnabled = enabled; }
    
    // Getters
    float getGravityScale() const { return gravityScale; }
    bool isUsingRK4() const { return useRK4; }
    
private:
    float gravityScale;
    bool useRK4;
    bool collisionsEnabled;
    
    // RK4 helper structures
    struct Derivative {
        glm::vec3 velocity;
        glm::vec3 acceleration;
    };
    
    Derivative evaluateDerivative(Body& body, float dt, const Derivative& d);
};

#endif