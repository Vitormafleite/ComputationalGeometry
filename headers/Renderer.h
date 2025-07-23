#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

#include <iostream>
#include <filesystem>

namespace fs =  std::filesystem;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(int width, int height, const char* title);

    void loadUIElements();
    void loadOBJFilesFromFolder(const std::string& folderPath);

    void beginFrame();
    void endFrame();
    void shutdown();

    bool shouldClose() const;
    GLFWwindow* getWindow() const;

    void loadMesh(const std::string& path);
    
    void setupPointCloud();
    
    void runMergeHull();
    void setupMiniHulls(int step);

    void renderGeometry();

    void ClearBuffers();

private:
    Mesh mesh_m;
    GLFWwindow* window;
    bool wireframeMode = false;
    bool rightMousePressed = false;

    std::vector<GLuint> pointCloudVAOs;
    std::vector<GLuint> pointCloudVBOs;
    std::vector<GLsizei> pointCloudVertexCounts;
    std::vector<GLuint> miniHullVAOs;
    std::vector<GLuint> miniHullVBOs;
    std::vector<GLsizei> miniHullVertexCounts;
    
    //Animate the merge process
    int mergeStep = 0;

    //Glfw stuff
    unsigned int samples = 8;
    
    //Just for imGui comboBox
    std::vector<std::string> objFileNames;

    //All shader stuff
    Shader shader;
    glm::mat4 view;
    glm::mat4 projection; 

    //All camera stuff
    Camera camera = Camera(glm::vec3(0.0f, 2.0f, 15.0f));
    float lastX = 400, lastY = 300;
    bool firstMouse = true;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    void handleCameraInput();
};

#endif // RENDERER_H