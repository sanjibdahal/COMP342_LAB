#include "PhysicsEngine.h"
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

PhysicsEngine::PhysicsEngine(float gravityScale)
    : gravityScale(gravityScale), useRK4(true), collisionsEnabled(true),
      dampingEnabled(false), dampingFactor(0.9999f),
      centeringForceEnabled(false), maxVelocity(50.0f) {
}

void PhysicsEngine::update(std::vector<std::unique_ptr<Body>>& bodies, float deltaTime) {
    // Limit deltaTime to prevent instability
    deltaTime = std::min(deltaTime, 0.05f);
    
    // Store center of mass for stability
    updateCenterOfMass(bodies);
    
    // Compute gravitational forces
    computeForces(bodies);
    
    // Integrate motion
    if (useRK4) {
        rk4Integration(bodies, deltaTime);
    } else {
        eulerIntegration(bodies, deltaTime);
    }
    
    // Apply stability constraints
    applyStabilityConstraints(bodies);
    
    // Handle collisions
    if (collisionsEnabled) {
        detectAndResolveCollisions(bodies);
    }
}

void PhysicsEngine::updateCenterOfMass(const std::vector<std::unique_ptr<Body>>& bodies) {
    centerOfMass = glm::vec3(0.0f);
    float totalMass = 0.0f;
    
    for (const auto& body : bodies) {
        centerOfMass += body->position * body->mass;
        totalMass += body->mass;
    }
    
    if (totalMass > 0.0f) {
        centerOfMass /= totalMass;
    }
}

void PhysicsEngine::applyStabilityConstraints(std::vector<std::unique_ptr<Body>>& bodies) {
    for (auto& body : bodies) {
        if (body->isFixed) continue;
        
        // Velocity limiting to prevent bodies from flying away
        float velocityMag = glm::length(body->velocity);
        if (velocityMag > maxVelocity) {
            body->velocity = glm::normalize(body->velocity) * maxVelocity;
        }
        
        // Optional: Apply slight damping for stability
        if (dampingEnabled) {
            body->velocity *= dampingFactor;
        }
        
        // Optional: Apply weak centering force to prevent drift
        if (centeringForceEnabled) {
            glm::vec3 toCenterOfMass = centerOfMass - body->position;
            float distanceFromCenter = glm::length(toCenterOfMass);
            
            if (distanceFromCenter > 200.0f) {
                // Apply very weak centering force
                glm::vec3 centeringForce = glm::normalize(toCenterOfMass) * 0.1f * body->mass;
                body->applyForce(centeringForce);
            }
        }
        
        // Prevent bodies from going too far from origin
        float distanceFromOrigin = glm::length(body->position);
        if (distanceFromOrigin > 500.0f) {
            // Gently push back towards center
            glm::vec3 directionToCenter = -glm::normalize(body->position);
            body->velocity += directionToCenter * 0.5f;
        }
    }
}

void PhysicsEngine::computeForces(std::vector<std::unique_ptr<Body>>& bodies) {
    // Reset all forces
    for (auto& body : bodies) {
        body->resetForces();
    }
    
    // Calculate gravitational forces between all pairs
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            glm::vec3 force = calculateGravitationalForce(*bodies[i], *bodies[j]);
            bodies[i]->applyForce(force);
            bodies[j]->applyForce(-force);
        }
    }
}

glm::vec3 PhysicsEngine::calculateGravitationalForce(const Body& b1, const Body& b2) const {
    glm::vec3 r = b2.position - b1.position;
    float distance = glm::length(r);
    
    // Prevent division by zero and singularities
    if (distance < SOFTENING) {
        distance = SOFTENING;
    }
    
    // Newton's law: F = G * m1 * m2 / r^2
    float forceMagnitude = (G * gravityScale * b1.mass * b2.mass) / (distance * distance);
    
    // Direction: unit vector from b1 to b2
    glm::vec3 direction = glm::normalize(r);
    
    return forceMagnitude * direction;
}

void PhysicsEngine::eulerIntegration(std::vector<std::unique_ptr<Body>>& bodies, float dt) {
    for (auto& body : bodies) {
        body->update(dt);
    }
}

void PhysicsEngine::rk4Integration(std::vector<std::unique_ptr<Body>>& bodies, float dt) {
    // Store initial states
    std::vector<State> initialStates;
    for (const auto& body : bodies) {
        State state;
        state.position = body->position;
        state.velocity = body->velocity;
        state.force = body->force;
        initialStates.push_back(state);
    }
    
    // RK4 integration for better accuracy
    for (size_t i = 0; i < bodies.size(); ++i) {
        auto& body = bodies[i];
        if (body->isFixed) continue;
        
        State s0 = initialStates[i];
        
        // k1
        Derivative k1;
        k1.velocity = s0.velocity;
        k1.acceleration = s0.force / body->mass;
        
        // k2
        body->position = s0.position + k1.velocity * (dt * 0.5f);
        body->velocity = s0.velocity + k1.acceleration * (dt * 0.5f);
        Derivative k2;
        k2.velocity = body->velocity;
        k2.acceleration = body->force / body->mass;
        
        // k3
        body->position = s0.position + k2.velocity * (dt * 0.5f);
        body->velocity = s0.velocity + k2.acceleration * (dt * 0.5f);
        Derivative k3;
        k3.velocity = body->velocity;
        k3.acceleration = body->force / body->mass;
        
        // k4
        body->position = s0.position + k3.velocity * dt;
        body->velocity = s0.velocity + k3.acceleration * dt;
        Derivative k4;
        k4.velocity = body->velocity;
        k4.acceleration = body->force / body->mass;
        
        // Combine
        glm::vec3 dPos = (k1.velocity + 2.0f*k2.velocity + 2.0f*k3.velocity + k4.velocity) / 6.0f;
        glm::vec3 dVel = (k1.acceleration + 2.0f*k2.acceleration + 2.0f*k3.acceleration + k4.acceleration) / 6.0f;
        
        // Update final state
        body->position = s0.position + dPos * dt;
        body->velocity = s0.velocity + dVel * dt;
        body->updateTrail();
    }
}

void PhysicsEngine::detectAndResolveCollisions(std::vector<std::unique_ptr<Body>>& bodies) {
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            if (checkCollision(*bodies[i], *bodies[j])) {
                resolveCollision(*bodies[i], *bodies[j]);
            }
        }
    }
}

bool PhysicsEngine::checkCollision(const Body& b1, const Body& b2) const {
    float distance = glm::length(b2.position - b1.position);
    return distance < (b1.radius + b2.radius);
}

void PhysicsEngine::resolveCollision(Body& b1, Body& b2) {
    // Elastic collision with momentum conservation
    glm::vec3 normal = glm::normalize(b2.position - b1.position);
    glm::vec3 relativeVel = b1.velocity - b2.velocity;
    float velAlongNormal = glm::dot(relativeVel, normal);
    
    // Don't resolve if velocities are separating
    if (velAlongNormal > 0) return;
    
    // Calculate restitution (1.0 = perfectly elastic)
    float restitution = 0.95f;
    
    // Calculate impulse scalar
    float j = -(1.0f + restitution) * velAlongNormal;
    j /= (1.0f / b1.mass + 1.0f / b2.mass);
    
    // Apply impulse
    glm::vec3 impulse = j * normal;
    if (!b1.isFixed) b1.velocity += impulse / b1.mass;
    if (!b2.isFixed) b2.velocity -= impulse / b2.mass;
    
    // Position correction to prevent overlap
    float penetration = (b1.radius + b2.radius) - glm::length(b2.position - b1.position);
    if (penetration > 0) {
        float totalMass = b1.mass + b2.mass;
        glm::vec3 correction = normal * penetration;
        if (!b1.isFixed) b1.position -= correction * (b2.mass / totalMass);
        if (!b2.isFixed) b2.position += correction * (b1.mass / totalMass);
    }
}

float PhysicsEngine::getTotalEnergy(const std::vector<std::unique_ptr<Body>>& bodies) const {
    float totalKE = 0.0f;
    float totalPE = 0.0f;
    
    for (const auto& body : bodies) {
        totalKE += body->getKineticEnergy();
    }
    
    // Potential energy between all pairs
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            glm::vec3 r = bodies[j]->position - bodies[i]->position;
            float distance = glm::length(r);
            if (distance > 0) {
                totalPE -= (G * gravityScale * bodies[i]->mass * bodies[j]->mass) / distance;
            }
        }
    }
    
    return totalKE + totalPE;
}

glm::vec3 PhysicsEngine::getTotalMomentum(const std::vector<std::unique_ptr<Body>>& bodies) const {
    glm::vec3 totalMomentum(0.0f);
    for (const auto& body : bodies) {
        totalMomentum += body->getMomentum();
    }
    return totalMomentum;
}

glm::vec3 PhysicsEngine::getCenterOfMass() const {
    return centerOfMass;
}