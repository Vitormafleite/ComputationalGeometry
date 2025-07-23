#ifndef WINGEDEDGEMESH_H
#define WINGEDEDGEMESH_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "Vector3.h"

struct Vertex {
    int id;
    int edgeId = -1;
    glm::vec3 position;
};

struct Face {
    int id;
    int edgeId = -1;
};

struct WingedEdge {
    int id;
    int vStartId = -1;
    int vEndId = -1;

    int fLeftId = -1;
    int fRightId = -1;

    int eLeftPrevId = -1;
    int eLeftNextId = -1;

    int eRightPrevId = -1;
    int eRightNextId = -1;
};

class WingedEdgeMesh {
public:
    std::vector<Vertex> vertices;
    std::vector<WingedEdge> edges;
    std::vector<Face> faces;

    int addVertex(const glm::vec3& pos);
    int addFace();
    int addEdge(int vStartId, int vEndId);

    int addFaceFromVertices(int a, int b, int c);

    std::vector<glm::vec3> extractTriangleVertices() const;

    void debugPrint() const;

    void clear();
};

#endif // WINGEDEDGEMESH_H
