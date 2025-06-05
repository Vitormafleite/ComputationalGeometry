#include "../headers/WingedEdgeMesh.h"

int WingedEdgeMesh::addVertex(const glm::vec3& pos) {
    int id = static_cast<int>(vertices.size());
    vertices.push_back(Vertex{ id, -1, pos });
    return id;
}

int WingedEdgeMesh::addFace() {
    int id = static_cast<int>(faces.size());
    faces.push_back(Face{ id, -1 });
    return id;
}

int WingedEdgeMesh::addEdge(int vStartId, int vEndId) {
    int id = static_cast<int>(edges.size());
    edges.push_back(WingedEdge{
        id,
        vStartId,
        vEndId,
        -1, -1, // fLeftId, fRightId
        -1, -1, // eLeftPrevId, eLeftNextId
        -1, -1  // eRightPrevId, eRightNextId
    });

    // Update vertex to reference one of its edges
    if (vertices[vStartId].edgeId == -1) {
        vertices[vStartId].edgeId = id;
    }

    return id;
}

int WingedEdgeMesh::addFaceFromVertices(int a, int b, int c) {
    int faceId = static_cast<int>(faces.size());
    Face face;
    face.id = faceId;
    faces.push_back(face);

    // Create the three edges for this triangle face
    int e0Id = static_cast<int>(edges.size());
    int e1Id = e0Id + 1;
    int e2Id = e0Id + 2;

    WingedEdge e0{e0Id, a, b, faceId}; // a→b
    WingedEdge e1{e1Id, b, c, faceId}; // b→c
    WingedEdge e2{e2Id, c, a, faceId}; // c→a

    // Set next/prev on the left (face side)
    e0.eLeftNextId = e1Id;
    e1.eLeftNextId = e2Id;
    e2.eLeftNextId = e0Id;

    e0.eLeftPrevId = e2Id;
    e1.eLeftPrevId = e0Id;
    e2.eLeftPrevId = e1Id;

    // Add to edge list
    edges.push_back(e0);
    edges.push_back(e1);
    edges.push_back(e2);

    // Update face to point to one of its edges
    faces[faceId].edgeId = e0Id;

    // Optional: point vertices to one of their outgoing edges
    if (vertices[a].edgeId == -1) vertices[a].edgeId = e0Id;
    if (vertices[b].edgeId == -1) vertices[b].edgeId = e1Id;
    if (vertices[c].edgeId == -1) vertices[c].edgeId = e2Id;

    return faceId;
}


void WingedEdgeMesh::debugPrint() const {
    std::cout << "=== Vertices ===\n";
    for (const auto& v : vertices) {
        std::cout << "Vertex " << v.id << ": ("
                  << v.position.x << ", " << v.position.y << ", " << v.position.z
                  << "), EdgeId: " << v.edgeId << '\n';
    }

    std::cout << "\n=== Faces ===\n";
    for (const auto& f : faces) {
        std::cout << "Face " << f.id << ", EdgeId: " << f.edgeId << '\n';
    }

    std::cout << "\n=== Edges ===\n";
    for (const auto& e : edges) {
        std::cout << "Edge " << e.id
                  << ": (" << e.vStartId << " -> " << e.vEndId << "), "
                  << "LeftFace: " << e.fLeftId << ", RightFace: " << e.fRightId << '\n';

        std::cout << "    LeftPrev: " << e.eLeftPrevId
                  << ", LeftNext: " << e.eLeftNextId << '\n';

        std::cout << "    RightPrev: " << e.eRightPrevId
                  << ", RightNext: " << e.eRightNextId << '\n';
    }
    std::cout << "=====================\n";
}

void WingedEdgeMesh::clear() {
    vertices.clear();
    edges.clear();
    faces.clear();
}
