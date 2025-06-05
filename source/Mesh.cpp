#include "../headers/Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0) {}

bool Mesh::loadFromOBJ(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << "\n";
        return false;
    }

    vertices.clear();
    indices.clear();

    std::vector<Vector3> temp_vertices;
    std::vector<unsigned int> temp_indices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            temp_vertices.emplace_back(x, y, z);
        } else if (prefix == "f") {
            std::vector<unsigned int> face_indices;
            std::string vertex_token;
            while (iss >> vertex_token) {
                std::istringstream vss(vertex_token);
                std::string index_str;
                std::getline(vss, index_str, '/');  // Get only the vertex index (ignore tex/norm)
                unsigned int idx = std::stoi(index_str);
                face_indices.push_back(idx - 1); // OBJ is 1-based
            }

            // Triangulate if more than 3 vertices in face
            for (size_t i = 1; i + 1 < face_indices.size(); ++i) {
                temp_indices.push_back(face_indices[0]);
                temp_indices.push_back(face_indices[i]);
                temp_indices.push_back(face_indices[i + 1]);
            }
        }
    }

    file.close();

    vertices = std::move(temp_vertices);
    indices = std::move(temp_indices);

    setupMesh();

    return true;
}

bool Mesh::loadSubmeshFromOBJ(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << "\n";
        return false;
    }

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

    std::cout << submeshesVertices.size() << std::endl;

    setupSubmeshRender();
    
    return true;
}

void Mesh::sortSubmeshes() {
    for (auto& submesh : submeshesVertices) {
        std::sort(submesh.begin(), submesh.end(), [](const Vector3& a, const Vector3& b) {
            return a.x < b.x;
        });
    }
}

//In case of errors check this
void Mesh::partitionSubmeshes() {
    partitions.clear();

    for (const auto& sortedSubmesh : submeshesVertices) {
        std::vector<std::vector<Vector3>> submeshPartitions;
        size_t total = sortedSubmesh.size();
        size_t i = 0;

        size_t numFives = total % 4;

        size_t numFours = (total - numFives * 5) / 4;

        for (size_t p = 0; p < numFives; ++p) {
            std::vector<Vector3> group(sortedSubmesh.begin() + i, sortedSubmesh.begin() + i + 5);
            submeshPartitions.push_back(group);
            i += 5;
        }

        for (size_t p = 0; p < numFours; ++p) {
            std::vector<Vector3> group(sortedSubmesh.begin() + i, sortedSubmesh.begin() + i + 4);
            submeshPartitions.push_back(group);
            i += 4;
        }

        //HANDLE THIS CASE LATER, POINTS <= 3
        if (i < total) {
            std::vector<Vector3> leftover(sortedSubmesh.begin() + i, sortedSubmesh.end());
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




void Mesh::setupMesh() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);

    glBindVertexArray(0);
}

void Mesh::setupSubmeshRender() {
    // Clean up old buffers
    for (GLuint vao : submeshVAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    for (GLuint vbo : submeshVBOs) {
        glDeleteBuffers(1, &vbo);
    }

    submeshVAOs.clear();
    submeshVBOs.clear();

    for (const auto& submesh : submeshesVertices) {
        GLuint vao, vbo;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, submesh.size() * sizeof(Vector3), submesh.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // location = 0 in vertex shader
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);

        glBindVertexArray(0);

        submeshVAOs.push_back(vao);
        submeshVBOs.push_back(vbo);
    }
}



void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

const std::vector<Vector3>& Mesh::getVertices() const { return vertices; }
const std::vector<unsigned int>& Mesh::getIndices() const { return indices; }
