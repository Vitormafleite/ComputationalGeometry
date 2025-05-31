#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : position(position), yaw(yaw), pitch(pitch), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(char direction, float deltaTime) {
    float velocity = speed * deltaTime;
    if (direction == 'F')
        position += front * velocity;
    if (direction == 'B')
        position -= front * velocity;
    if (direction == 'L')
        position -= right * velocity;
    if (direction == 'R')
        position += right * velocity;
    if (direction == 'U') 
        position += up * velocity;
    if (direction == 'D') 
        position -= up * velocity;
}

void Camera::processMouse(float xOffset, float yOffset) {
    xOffset *= sensitivity;
    yOffset *= -sensitivity; // negative sign to invert y-axis movement for better control

    yaw += xOffset;
    pitch -= yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

glm::vec3 Camera::getPosition() const {
    return position;
}

void Camera::updateCameraVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
