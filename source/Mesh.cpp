#include "../headers/Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0) {}

bool Mesh::loadSubmeshFromOBJ(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << "\n";
        return false;
    }

    ClearMeshData();

    std::vector<Vector3> allVertices;
    std::vector<Vector3> currentSubmesh;
    std::set<unsigned int> currentIndices;
    std::string line;

    bool hasStarted = false;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "o" || prefix == "g") {
            if (hasStarted && !currentIndices.empty()) {
                // Finish previous submesh
                std::vector<Vector3> submesh;
                for (unsigned int idx : currentIndices)
                    submesh.push_back(allVertices[idx]);

                submeshesVertices.push_back(std::move(submesh));
                currentIndices.clear();
            }
            hasStarted = true;
        } else if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            allVertices.emplace_back(x, y, z);
        } else if (prefix == "f") {
            std::string vertex_token;
            while (iss >> vertex_token) {
                std::istringstream vss(vertex_token);
                std::string index_str;
                std::getline(vss, index_str, '/');
                unsigned int idx = std::stoi(index_str);
                currentIndices.insert(idx - 1); // OBJ indices are 1-based
            }
        }
    }

    // Add the last submesh
    if (!currentIndices.empty()) {
        std::vector<Vector3> submesh;
        for (unsigned int idx : currentIndices)
            submesh.push_back(allVertices[idx]);

        submeshesVertices.push_back(std::move(submesh));
    }

    file.close();

    //setupSubmeshRender();
    
    return true;
}

void Mesh::sortSubmeshes() {
    for (auto& submesh : submeshesVertices) {
        std::sort(submesh.begin(), submesh.end(), [](const Vector3& a, const Vector3& b) {
            return a.x < b.x;
        });
    }
}

//Might have some errors here
void Mesh::partitionSubmeshes() {
    partitions.clear();
    const float epsilon = 1e-6f;

    auto areEqual = [epsilon](const Vector3& a, const Vector3& b) {
        return glm::length(glm::vec3(a.x - b.x, a.y - b.y, a.z - b.z)) < epsilon;
    };

    for (const auto& sortedSubmesh : submeshesVertices) {
        std::vector<Vector3> deduplicated;
        for (const auto& pt : sortedSubmesh) {
            bool isUnique = true;
            for (const auto& existing : deduplicated) {
                if (areEqual(pt, existing)) {
                    isUnique = false;
                    break;
                }
            }
            if (isUnique) {
                deduplicated.push_back(pt);
            }
        }

        std::vector<std::vector<Vector3>> submeshPartitions;
        size_t total = deduplicated.size();
        size_t i = 0;

        size_t numFives = total % 4;
        size_t numFours = (total - numFives * 5) / 4;

        for (size_t p = 0; p < numFives; ++p) {
            if (i + 5 <= total) {
                std::vector<Vector3> group(deduplicated.begin() + i, deduplicated.begin() + i + 5);
                submeshPartitions.push_back(group);
                i += 5;
            }
        }

        for (size_t p = 0; p < numFours; ++p) {
            if (i + 4 <= total) {
                std::vector<Vector3> group(deduplicated.begin() + i, deduplicated.begin() + i + 4);
                submeshPartitions.push_back(group);
                i += 4;
            }
        }

        // Handle remaining vertices (e.g., 3 or fewer)
        if (i < total) {
            std::vector<Vector3> leftover(deduplicated.begin() + i, deduplicated.end());
            if (!leftover.empty())
                submeshPartitions.push_back(leftover);
        }

        partitions.push_back(submeshPartitions);
    }
}

void Mesh::buildPartitionConvexHulls() {
    localHulls.clear();
    for (const auto& submeshPartitions : partitions) {
        std::vector<WingedEdgeMesh> hullsForSubmesh;
        for (const auto& group : submeshPartitions) {
            hullsForSubmesh.push_back(buildInitialHull(group));
        }
        localHulls.push_back(std::move(hullsForSubmesh));
    }

    hullsSteps.push_back(localHulls);
}

WingedEdgeMesh Mesh::buildInitialHull(const std::vector<Vector3>& group) {
    WingedEdgeMesh hull;
    std::vector<int> vIds;

    for (const auto& pt : group) {
        glm::vec3 gpt(pt.x, pt.y, pt.z);
        vIds.push_back(hull.addVertex(gpt));
    }

    if (group.size() == 4) {
        const float epsilon = 1e-6f;

        for (int i = 0; i < 4; ++i) {
            int a = (i + 1) % 4;
            int b = (i + 2) % 4;
            int c = (i + 3) % 4;
            int test = i;

            glm::vec3 A(group[a].x, group[a].y, group[a].z);
            glm::vec3 B(group[b].x, group[b].y, group[b].z);
            glm::vec3 C(group[c].x, group[c].y, group[c].z);
            glm::vec3 N = glm::normalize(glm::cross(B - A, C - A));

            glm::vec3 T(group[test].x, group[test].y, group[test].z);
            float d = glm::dot(T - A, N);

            if (d > epsilon) {
                hull.addFaceFromVertices(vIds[a], vIds[c], vIds[b]);
            } else {
                hull.addFaceFromVertices(vIds[a], vIds[b], vIds[c]);
            }
        }
    }
    else if (group.size() == 5) {
        const float epsilon = 1e-6f;
        std::set<std::tuple<int, int, int>> uniqueFaces;

        for (int i = 0; i < 5; ++i) {
            for (int j = i + 1; j < 5; ++j) {
                for (int k = j + 1; k < 5; ++k) {
                    glm::vec3 A(group[i].x, group[i].y, group[i].z);
                    glm::vec3 B(group[j].x, group[j].y, group[j].z);
                    glm::vec3 C(group[k].x, group[k].y, group[k].z);
                    glm::vec3 N = glm::normalize(glm::cross(B - A, C - A));
                    if (glm::length(N) < epsilon) continue;

                    bool allNeg = true, allPos = true;
                    for (int l = 0; l < 5; ++l) {
                        if (l == i || l == j || l == k) continue;
                        glm::vec3 T(group[l].x, group[l].y, group[l].z);
                        float dot = glm::dot(T - A, N);
                        allNeg &= (dot < -epsilon);
                        allPos &= (dot > epsilon);
                    }

                    if (allNeg || allPos) {
                        int a = vIds[i], b = vIds[j], c = vIds[k];
                        if (allNeg) std::swap(b, c);
                        auto sorted = std::minmax({a, b, c});
                        auto key = std::make_tuple(sorted.first, sorted.second, a + b + c - sorted.first - sorted.second);
                        if (uniqueFaces.insert(key).second) {
                            hull.addFaceFromVertices(a, b, c);
                        }
                    }
                }
            }
        }
    }

    return hull;
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

const std::vector<Vector3>& Mesh::getVertices() const { return vertices; }
const std::vector<unsigned int>& Mesh::getIndices() const { return indices; }

void Mesh::ClearMeshData(){
    vertices.clear(); 
    submeshesVertices.clear();
    partitions.clear(); 
    localHulls.clear();
    hullsSteps.clear();
    indices.clear();
};