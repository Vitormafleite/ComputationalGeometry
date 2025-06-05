#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <numeric>

#include "Vector3.h"
#include "WingedEdgeMesh.h"

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
    bool loadSubmeshFromOBJ(const std::string& filepath);
    void sortSubmeshes();
    void partitionSubmeshes();
    void buildPartitionConvexHulls();
    WingedEdgeMesh buildInitialHull(const std::vector<Vector3>& group);

    void draw() const;
    
    std::vector<Vector3> vertices; // List all vertices 
    std::vector<GLuint> submeshVAOs;
    std::vector<GLuint> submeshVBOs;
    std::vector<std::vector<Vector3>> submeshesVertices; // Divide vertices into submeshes according to the .obj folder // Usign this for renderign as well
    std::vector<std::vector<std::vector<Vector3>>> partitions; // After ordering allong the x-axis, divide the submeshes into partitions
    std::vector<std::vector<WingedEdgeMesh>> localHulls;
    
private:

    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;

    void setupMesh();
    void setupSubmeshRender() ;
};

#endif //MESH_H