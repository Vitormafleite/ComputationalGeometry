#include "../headers/Renderer.h"
#include <iostream>

float triangle[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

Renderer::Renderer() : window(nullptr) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glEnable(GL_DEPTH_TEST);
    if (!shader.load("../shaders/shader.vert", "../shaders/shader.frag")) {
        std::cerr << "Failed to load shaders!\n";
        return false;
    }

    view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    return true;
}

void Renderer::beginFrame() {
    glfwPollEvents();

    glClearColor(0.752f, 0.752f, 0.752f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::loadUIElements() {
    static char objPath[256] = "";
    static bool loadRequested = false;

    ImGui::Begin("OBJ Loader");
    ImGui::InputText("OBJ File Path", objPath, IM_ARRAYSIZE(objPath));
    if (ImGui::Button("Load OBJ")) {
        loadRequested = true;
    }
    ImGui::End();

    if (loadRequested) {
        loadMesh(objPath);
        loadRequested = false;
    }
}


void Renderer::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void Renderer::shutdown() {
    if (window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

bool Renderer::shouldClose() const {
    return window && glfwWindowShouldClose(window);
}

GLFWwindow* Renderer::getWindow() const {
    return window;
}

void Renderer::loadMesh(const std::string& path) {
    if (path.empty()) {
        std::cerr << "Path is empty. Not attempting to load.\n";
        return;
    }

    if (!mesh_m.loadFromOBJ(path)) {
        std::cerr << "Failed to load mesh: " << path << std::endl;
    } else {
        std::cout << "Loaded mesh: " << path << std::endl;
    }
}

void Renderer::renderMesh() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe

    shader.use();
    shader.setMat4("uModel", glm::mat4(1.0f));
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    mesh_m.draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore to normal
}

void Renderer::drawTestTriangle() {
    float triangle[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int testVAO, testVBO;
    glGenVertexArrays(1, &testVAO);
    glGenBuffers(1, &testVBO);

    glBindVertexArray(testVAO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    shader.use();
    shader.setMat4("uModel", glm::mat4(1.0f));
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);

    glBindVertexArray(testVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // Cleanup to not leak VAO/VBO
    glDeleteBuffers(1, &testVBO);
    glDeleteVertexArrays(1, &testVAO);
}
