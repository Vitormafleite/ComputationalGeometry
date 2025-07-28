#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position, float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    void processKeyboard(char direction, float deltaTime);
    void processMouse(float xOffset, float yOffset);

    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float speed = 6.0f;
    float sensitivity = 0.2f;

    void updateCameraVectors();
};

#endif
