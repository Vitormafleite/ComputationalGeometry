#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

#include <iostream>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(int width, int height, const char* title);
    void loadUIElements();
    void beginFrame();
    void endFrame();
    void shutdown();

    bool shouldClose() const;
    GLFWwindow* getWindow() const;

    void loadMesh(const std::string& path);
    void renderMesh();

    void drawTestTriangle();

private:
    Mesh mesh_m;
    GLFWwindow* window;
    bool wireframeMode = false;
    bool rightMousePressed = false;

    //All shader stuff
    Shader shader;
    glm::mat4 view;
    glm::mat4 projection; 

    //All camera stuff
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
    float lastX = 400, lastY = 300;
    bool firstMouse = true;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    void handleCameraInput();
};

#endif // RENDERER_H