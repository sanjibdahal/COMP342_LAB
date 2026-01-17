#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, float fov)
    : position(position), target(target), fov(fov),
      worldUp(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f),
      nearPlane(0.1f), farPlane(10000.0f), mode(Mode::ORBIT),
      rotationSpeed(0.2f), panSpeed(0.1f), zoomSpeed(2.0f) {
    distance = glm::length(position - target);
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::rotate(float yawOffset, float pitchOffset) {
    yaw += yawOffset * rotationSpeed;
    pitch += pitchOffset * rotationSpeed;
    
    // Constrain pitch
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    
    updateVectors();
}

void Camera::pan(float xOffset, float yOffset) {
    target += right * xOffset * panSpeed * distance * 0.01f;
    target += up * yOffset * panSpeed * distance * 0.01f;
    updateVectors();
}

void Camera::zoom(float amount) {
    distance -= amount * zoomSpeed;
    distance = std::max(1.0f, distance);
    updateVectors();
}

void Camera::setTarget(const glm::vec3& newTarget) {
    target = newTarget;
    updateVectors();
}

void Camera::setMode(Mode newMode) {
    mode = newMode;
}

void Camera::update(float deltaTime) {
    // Can add smooth camera movement here if needed
}

void Camera::updateVectors() {
    // Calculate camera position based on yaw, pitch, and distance
    glm::vec3 newPosition;
    newPosition.x = target.x + distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    newPosition.y = target.y + distance * sin(glm::radians(pitch));
    newPosition.z = target.z + distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    position = newPosition;
    
    // Calculate direction vectors
    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}