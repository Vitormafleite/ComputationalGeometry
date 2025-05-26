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

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

const std::vector<Vector3>& Mesh::getVertices() const { return vertices; }
const std::vector<unsigned int>& Mesh::getIndices() const { return indices; }
