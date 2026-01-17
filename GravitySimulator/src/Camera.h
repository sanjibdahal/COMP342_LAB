#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Camera modes
    enum class Mode {
        FREE,
        FOLLOW,
        ORBIT
    };
    
    // Constructor
    Camera(const glm::vec3& position = glm::vec3(0.0f, 50.0f, 100.0f),
           const glm::vec3& target = glm::vec3(0.0f, 0.0f, 0.0f),
           float fov = 45.0f);
    
    // View matrix
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // Camera controls
    void rotate(float yaw, float pitch);
    void pan(float x, float y);
    void zoom(float amount);
    void setTarget(const glm::vec3& target);
    void setMode(Mode mode);
    
    // Update
    void update(float deltaTime);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getTarget() const { return target; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    float getDistance() const { return distance; }
    
    // Setters
    void setPosition(const glm::vec3& pos) { position = pos; updateVectors(); }
    void setDistance(float dist) { distance = dist; updateVectors(); }
    
private:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // Euler angles
    float yaw;
    float pitch;
    float distance;
    
    // Camera options
    float fov;
    float nearPlane;
    float farPlane;
    Mode mode;
    
    // Movement speeds
    float rotationSpeed;
    float panSpeed;
    float zoomSpeed;
    
    // Helper methods
    void updateVectors();
};

#endif