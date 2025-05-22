#ifndef RENDERER_H
#define RENDERER_H

#include "../external/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../external/imgui/imgui.h"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"


#include <iostream>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(int width, int height, const char* title);
    void beginFrame();
    void endFrame();
    void shutdown();

    bool shouldClose() const;
    GLFWwindow* getWindow() const;

private:

    GLFWwindow* window;
    
};

#endif // RENDERER_H