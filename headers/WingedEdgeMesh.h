#ifndef WINGEDEDGEMESH_H
#define WINGEDEDGEMESH_H

#include <vector>
#include <algorithm>
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
    glm::vec3 faceNormal;

    Face() : id(-1), edgeId(-1), faceNormal(glm::vec3(0.0f, 0.0f, 0.0f)) {}

    Face(int id, int edgeID, glm::vec3 faceNormal) : id(id), edgeId(edgeID), faceNormal(faceNormal) {}
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

    WingedEdge()
        : id(-1), vStartId(-1), vEndId(-1),
          fLeftId(-1), fRightId(-1),
          eLeftPrevId(-1), eLeftNextId(-1),
          eRightPrevId(-1), eRightNextId(-1) {}

    WingedEdge(int id, int vStartId, int vEndId)
        : id(id), vStartId(vStartId), vEndId(vEndId),
          fLeftId(-1), fRightId(-1),
          eLeftPrevId(-1), eLeftNextId(-1),
          eRightPrevId(-1), eRightNextId(-1) {}
 
};

class WingedEdgeMesh {
public:
    std::vector<Vertex> vertices;
    std::vector<WingedEdge> edges;
    std::vector<Face> faces;

    std::vector<int> openEdgesQueue; // while we build the winged edge table, edges only get half built when a face is inserted, so we need to track that and fill them later 
    
    //use this for merging only
    std::vector<int> leftHullOpenEdgesQueue;
    std::vector<int> rightHullOpenEdgesQueue;

    int AddVertex(const glm::vec3& pos);

    void AddFaceFromVertices(int a, int b, int c);

    std::vector<glm::vec3> ExtractVerticesPositions(); 

    void DeleteFace(int faceID);
    void DeleteFaceFromEdge(int faceID, int EdgeID, bool isLeftFace);

    void FixMeshAfterDeletions();
    void AppendDataToLinkHulls(int verticesAmount, int edgesAmount, int facesAmount);
    void AddFirstMergingEdge();
    void OrderLeftAndRightQueues(int startVertexLeftId, int StartVertexRightId, int startEdgeLeftId, int startEdgeRightId);

    int CheckHowManyEdgesTriangleRemovesFromQueue(int a, int b, int c);

    std::vector<glm::vec3> ExtractTriangleVertices() const;

    void DebugPrint() const;

    void Clear();
};

#endif // WINGEDEDGEMESH_H
