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
    glfwWindowHint(GLFW_SAMPLES, samples);

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
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    if (!shader.load("../shaders/shader.vert", "../shaders/shader.frag")) {
        std::cerr << "Failed to load shaders!\n";
        return false;
    }

    view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -20.0f));
    projection = glm::perspective(glm::radians(45.0f), 1200.0f / 900.0f, 0.1f, 100.0f);

    return true;
}

void Renderer::beginFrame() {
    glfwPollEvents();

    handleCameraInput();\

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

    ImGui::Checkbox("Wireframe Mode", &wireframeMode);
    ImGui::End();

    if (loadRequested) {
        loadMesh(objPath);
        loadRequested = false;
    }

    glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
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

    if (!mesh_m.loadSubmeshFromOBJ(path)) {
        std::cerr << "Failed to load mesh: " << path << std::endl;
    } else {
        std::cout << "Loaded mesh: " << path << std::endl;
    }
}

void Renderer::renderMesh() {
    for (int i = 0; i < mesh_m.submeshVAOs.size(); ++i) {
        glBindVertexArray(mesh_m.submeshVAOs[i]);
        glDrawArrays(GL_POINTS, 0, mesh_m.submeshesVertices[i].size());
    }
    glBindVertexArray(0);

    shader.use();
    shader.setMat4("uModel", glm::mat4(1.0f));
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);


    GLint pointSizeLoc = glGetUniformLocation(shader.getID(), "uPointSize");
    glUniform1f(pointSizeLoc, 6.0f);

    mesh_m.draw();
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

    glBindVertexArray(testVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // Cleanup to not leak VAO/VBO
    glDeleteBuffers(1, &testVBO);
    glDeleteVertexArrays(1, &testVAO);
}

void Renderer::handleCameraInput() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Keyboard input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard('F', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard('B', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard('L', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard('R', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.processKeyboard('U', deltaTime); 
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.processKeyboard('D', deltaTime); 

    // Right mouse button pressed -> lock cursor
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !rightMousePressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        rightMousePressed = true;
        firstMouse = true; // reset to avoid jump
    }

    // Right mouse button released -> unlock cursor
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && rightMousePressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        rightMousePressed = false;
    }

    // Only handle mouse movement if right mouse is held
    if (rightMousePressed) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xOffset = xpos - lastX;
        float yOffset = lastY - ypos; // Inverted Y
        lastX = xpos;
        lastY = ypos;

        camera.processMouse(xOffset, yOffset);
    }

    // Update the view matrix
    view = camera.getViewMatrix();
}

