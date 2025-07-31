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

    bool loadSubmeshFromOBJ(const std::string& filepath);
    void sortSubmeshes();
    void partitionSubmeshes();
    void buildPartitionConvexHulls();

    WingedEdgeMesh InitialHull(const std::vector<Vector3>& group);

    bool FaceSeesOtherHull(glm::vec3 faceNormal, glm::vec3 faceVertex, std::vector<glm::vec3> otherHullVertices);

    void MergeHull();

    WingedEdgeMesh MergeTwoHulls(WingedEdgeMesh leftHull, WingedEdgeMesh rightHull);
    WingedEdgeMesh LinkHulls(WingedEdgeMesh leftHull, WingedEdgeMesh rightHull);

    void ClearMeshData();
    
    std::vector<Vector3> vertices; // List all vertices 
    std::vector<std::vector<Vector3>> submeshesVertices; // Divide vertices into submeshes according to the .obj folder // Using this for renderign as well
    std::vector<std::vector<std::vector<Vector3>>> partitions; // After ordering allong the x-axis, divide the submeshes into partitions
    std::vector<std::vector<WingedEdgeMesh>> localHulls;
    
    std::vector<std::vector<std::vector<WingedEdgeMesh>>> mergeHullPartitionsColections;
    std::vector<unsigned int> indices;
    
private:

};

#endif //MESH_H