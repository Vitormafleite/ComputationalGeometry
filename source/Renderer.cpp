#include "../headers/Renderer.h"
#include <iostream>

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
        glfwTerminate();
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Enable face culling and set CCW as front face
    glDisable(GL_CULL_FACE);
    //glCullFace(GL_BACK); // Cull back faces
    //glFrontFace(GL_CCW); // Counter-clockwise winding is front face
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    if (!shader.load("../shaders/shader.vert", "../shaders/shader.frag")) {
        std::cerr << "Failed to load shaders!\n";
        return false;
    }

    view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -20.0f));
    projection = glm::perspective(glm::radians(45.0f), 1200.0f / 900.0f, 0.1f, 100.0f);

    // Initialize the grid
    createGrid();

    return true;
}

void Renderer::createGrid() {
    const int gridSize = 20; // Number of grid lines on each side
    const float gridSpacing = 1.0f; // Distance between grid lines
    const float gridExtent = gridSize * gridSpacing; // Total size of the grid
    
    std::vector<glm::vec3> gridVertices;
    
    // Create grid lines parallel to X-axis (constant Z)
    for (int i = -gridSize; i <= gridSize; ++i) {
        float z = i * gridSpacing;
        // Start and end points for this line
        gridVertices.push_back(glm::vec3(-gridExtent, 0.0f, z));
        gridVertices.push_back(glm::vec3(gridExtent, 0.0f, z));
    }
    
    // Create grid lines parallel to Z-axis (constant X)
    for (int i = -gridSize; i <= gridSize; ++i) {
        float x = i * gridSpacing;
        // Start and end points for this line
        gridVertices.push_back(glm::vec3(x, 0.0f, -gridExtent));
        gridVertices.push_back(glm::vec3(x, 0.0f, gridExtent));
    }
    
    // Generate and bind VAO/VBO for grid
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(glm::vec3), gridVertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    gridVertexCount = static_cast<GLsizei>(gridVertices.size());
}

void Renderer::loadOBJFilesFromFolder(const std::string& folderPath) {
    objFileNames.clear();
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".obj") {
            objFileNames.push_back(entry.path().filename().string());
        }
    }
}

void Renderer::beginFrame() {
    glfwPollEvents();

    handleCameraInput();\

    glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // grey color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void Renderer::loadUIElements() {
    static char objPath[256] = "";
    static bool loadRequested = false;
    static bool runMerge = false;

    static int selectedIndex = 0;

    ImGui::Begin("OBJ Loader");

    //ImGui::InputText("OBJ File Path", objPath, IM_ARRAYSIZE(objPath));
    loadOBJFilesFromFolder("../assets");

    if (ImGui::BeginCombo("Select OBJ File", objFileNames[selectedIndex].c_str())) {
    for (int i = 0; i < objFileNames.size(); ++i) {
        bool isSelected = (selectedIndex == i);
        if (ImGui::Selectable(objFileNames[i].c_str(), isSelected)) {
            selectedIndex = i;
            // Optionally load the file here
        }
        if (isSelected)
            ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
}

    if (ImGui::Button("Load OBJ")) {
        loadRequested = true;
    }

    ImGui::Checkbox("Wireframe Mode", &wireframeMode);

    if (ImGui::Button("Start Merge")) {
        runMerge = true;
    }

    if (ImGui::Button("Previous Merge Step")) {
        if(mergeStep > 0){
            mergeStep--;
            setupMiniHulls(mergeStep);
        }
    }
    
    if (ImGui::Button("Next Merge Step")) {
        if(mergeStep < maxMergeStep){
            mergeStep++;
            setupMiniHulls(mergeStep);
        }
    }

    ImGui::End();

    if (loadRequested) {
        if (!objFileNames.empty()) {
            std::string fullPath = "../assets/" + objFileNames[selectedIndex];
            loadMesh(fullPath);
        }

        loadRequested = false;
    }

    if (runMerge) {
        runMergeHull();
        runMerge = false;
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
        glfwMakeContextCurrent(window);
        
        shader.cleanup();
        ClearBuffers();

        // Clean up grid buffers (not handled by ClearBuffers)
        if (gridVAO != 0) {
            glDeleteVertexArrays(1, &gridVAO);
            gridVAO = 0;
        }
        if (gridVBO != 0) {
            glDeleteBuffers(1, &gridVBO);
            gridVBO = 0;
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
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

    ClearBuffers();

    if (!mesh_m.loadSubmeshFromOBJ(path)) {
        std::cerr << "Failed to load mesh: " << path << std::endl;
    } else {
        std::cout << "Loaded mesh: " << path << std::endl;
        setupPointCloud();
    }
}

void Renderer::runMergeHull() {
    if (mesh_m.submeshesVertices.size() == 0){
        std::cerr << "The mesh has no set of points \n";
        return;
    }

    mesh_m.sortSubmeshes();
    mesh_m.partitionSubmeshes();
    mesh_m.buildPartitionConvexHulls();

    mesh_m.MergeHull();

    maxMergeStep = mesh_m.mergeHullPartitionsColections.size() - 1;

    setupMiniHulls(0);
    mergeStep = 0;
}


void Renderer::setupPointCloud(){
    // Clean up old buffers
    for (GLuint vao : pointCloudVAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    for (GLuint vbo : pointCloudVBOs) {
        glDeleteBuffers(1, &vbo);
    }
    
    pointCloudVAOs.clear();
    pointCloudVBOs.clear();
    
    for (const auto& submesh : mesh_m.submeshesVertices) {
        GLuint vao, vbo;
        
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, submesh.size() * sizeof(Vector3), submesh.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0); 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        pointCloudVAOs.push_back(vao);
        pointCloudVBOs.push_back(vbo);
        pointCloudVertexCounts.push_back(static_cast<GLsizei>(submesh.size()));
    }
}

void Renderer::setupMiniHulls(int step) {
    // Cleanup anterior
    for (GLuint vao : miniHullVAOs) glDeleteVertexArrays(1, &vao);
    for (GLuint vbo : miniHullVBOs) glDeleteBuffers(1, &vbo);
    
    miniHullVAOs.clear();
    miniHullVBOs.clear();
    miniHullVertexCounts.clear();
    
    //IMPLEMENT STEP LOGIC FOR ANIMATION

    for (const auto& meshGroup : mesh_m.mergeHullPartitionsColections[step]) {
        for (const auto& mesh : meshGroup) {
            std::vector<glm::vec3> vertices = mesh.ExtractTriangleVertices();
            if (vertices.empty()) continue;
            
            GLuint vao, vbo;
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0); // aPos
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            
            glBindVertexArray(0);
            
            miniHullVAOs.push_back(vao);
            miniHullVBOs.push_back(vbo);
            miniHullVertexCounts.push_back(static_cast<GLsizei>(vertices.size()));
        }
    }
}

void Renderer::renderGeometry() {
    shader.use();
    shader.setMat4("uModel", glm::mat4(1.0f));
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    
    // Render the grid first (behind everything else)
    if (gridVAO != 0 && gridVertexCount > 0) {
        shader.setBool("uRenderingPoints", false);
        shader.setBool("uWireframePass", false);
        shader.setBool("uRenderingGrid", true);
        
        // Ensure grid is rendered with proper depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridVertexCount);
        glBindVertexArray(0);
        
        shader.setBool("uRenderingGrid", false);
    }
    
    shader.setBool("uRenderingPoints", true);
    for (size_t i = 0; i < pointCloudVAOs.size(); ++i) {
        glBindVertexArray(pointCloudVAOs[i]);
        glDrawArrays(GL_POINTS, 0, pointCloudVertexCounts[i]);
    }
  
    shader.setBool("uRenderingPoints", false);
    shader.setBool("uWireframePass", false);

    if (wireframeMode) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (size_t i = 0; i < miniHullVAOs.size(); ++i) {
            glBindVertexArray(miniHullVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, miniHullVertexCounts[i]);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
    } else {
        // 1. Render filled polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for (size_t i = 0; i < miniHullVAOs.size(); ++i) {
            glBindVertexArray(miniHullVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, miniHullVertexCounts[i]);
        }

        // 2. Render wireframe overlay
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f); // pull forward to avoid z-fighting
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.setBool("uWireframePass", true);
        for (size_t i = 0; i < miniHullVAOs.size(); ++i) {
            glBindVertexArray(miniHullVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, miniHullVertexCounts[i]);
        }

        // Reset
        shader.setBool("uWireframePass", false);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
    }

    glBindVertexArray(0);
}

void Renderer::ClearBuffers(){
    // Delete point cloud buffers
    for (GLuint vao : pointCloudVAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    for (GLuint vbo : pointCloudVBOs) {
        glDeleteBuffers(1, &vbo);
    }
    
    // Delete mini hull buffers
    for (GLuint vao : miniHullVAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    for (GLuint vbo : miniHullVBOs) {
        glDeleteBuffers(1, &vbo);
    }

    // Note: Grid buffers are NOT deleted here because the grid should persist
    // throughout the application lifecycle

    pointCloudVAOs.clear();
    pointCloudVBOs.clear();
    pointCloudVertexCounts.clear();
    miniHullVAOs.clear();
    miniHullVBOs.clear();
    miniHullVertexCounts.clear();
    // gridVertexCount is not reset here as the grid should remain
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