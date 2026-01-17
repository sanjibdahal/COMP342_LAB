#include "Body.h"
#include <glm/glm.hpp>

Body::Body(const glm::vec3& pos, const glm::vec3& vel, float m, float r,
           const glm::vec3& col, const std::string& n, bool fixed)
    : position(pos), velocity(vel), mass(m), radius(r), color(col),
      name(n), isFixed(fixed), maxTrailLength(500), isSelected(false) {
    acceleration = glm::vec3(0.0f);
    force = glm::vec3(0.0f);
}

void Body::applyForce(const glm::vec3& f) {
    force += f;
}

void Body::update(float deltaTime) {
    if (isFixed) return;
    
    // F = ma => a = F/m
    acceleration = force / mass;
    
    // Update velocity and position
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    
    // Update trail
    updateTrail();
}

void Body::resetForces() {
    force = glm::vec3(0.0f);
}

void Body::updateTrail() {
    trail.push_back(position);
    if (trail.size() > maxTrailLength) {
        trail.erase(trail.begin());
    }
}

void Body::clearTrail() {
    trail.clear();
}

float Body::getKineticEnergy() const {
    float v2 = glm::dot(velocity, velocity);
    return 0.5f * mass * v2;
}

float Body::getPotentialEnergy(const std::vector<Body*>& others, float G) const {
    float potential = 0.0f;
    for (const Body* other : others) {
        if (other == this) continue;
        
        glm::vec3 r = other->position - position;
        float distance = glm::length(r);
        if (distance > 0.0f) {
            potential -= G * mass * other->mass / distance;
        }
    }
    return potential * 0.5f; // Divide by 2 to avoid double counting
}

glm::vec3 Body::getMomentum() const {
    return mass * velocity;
}