#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.h"
#include "Shader.h"

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

    //All shader stuff
    Shader shader;
    glm::mat4 view;
    glm::mat4 projection; 
};

#endif // RENDERER_H