#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include "Vector3.h"

#include "../external/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../external/imgui/imgui.h"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"

class Mesh{
public:

    Mesh();

    const std::vector<Vector3>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;

    bool loadFromOBJ(const std::string& filepath);
    void draw() const;

private:

    std::vector<Vector3> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

#endif //MESH_H