#include "../headers/WingedEdgeMesh.h"

int WingedEdgeMesh::addVertex(const glm::vec3& pos) {
    int id = static_cast<int>(vertices.size());
    vertices.push_back(Vertex{ id, -1, pos });
    return id;
}


int WingedEdgeMesh::checkHowManyEdgesTriangleRemovesFromQueue(int vertexA, int vertexB, int vertexC){
    int edgesTableHighestID = edges.size() - 1;

    int edgeABID, edgeBCID, edgeCAID;
    int edgesFoundCounter = 0;

    //REFACTOR THIS BS LATER CAUSE ITS THE SAME FOR THE 3 EDGES, NO TIME RIGHT NOW
    int edgeTableIndex = 0;

    bool foundEdgeAB = false;
    bool foundEdgeBC = false;
    bool foundEdgeCA = false;
    while(edgeTableIndex < this->edges.size()){

        if(foundEdgeAB && foundEdgeBC && foundEdgeCA)
            break;

        if(!foundEdgeAB){
            if ((edges[edgeTableIndex].vStartId == vertexA or edges[edgeTableIndex].vStartId == vertexB) && (edges[edgeTableIndex].vEndId == vertexA or edges[edgeTableIndex].vEndId == vertexB)){
                foundEdgeAB = true;
                edgesFoundCounter++;
            }
        }

        if(!foundEdgeBC){
            if ((edges[edgeTableIndex].vStartId == vertexB or edges[edgeTableIndex].vStartId == vertexC) && (edges[edgeTableIndex].vEndId == vertexB or edges[edgeTableIndex].vEndId == vertexC)){
                foundEdgeBC = true;
                edgesFoundCounter++;
            }
        }

        if(!foundEdgeCA){
            if ((edges[edgeTableIndex].vStartId == vertexC or edges[edgeTableIndex].vStartId == vertexA) && (edges[edgeTableIndex].vEndId == vertexC or edges[edgeTableIndex].vEndId == vertexA)){
                foundEdgeCA = true;
                edgesFoundCounter++;
            }
        }

        edgeTableIndex++;
    }

    return edgesFoundCounter;

}

int WingedEdgeMesh::addFaceFromVertices(int vertexA, int vertexB, int vertexC) {
    int edgesTableHighestID = edges.size() - 1;

    int edgeABID, edgeBCID, edgeCAID;

    //REFACTOR THIS BS LATER CAUSE ITS THE SAME FOR THE 3 EDGES, NO TIME RIGHT NOW
    int edgeTableIndex = 0;
    bool foundEdgeAB = false;
    bool foundEdgeBC = false;
    bool foundEdgeCA = false;
    while(edgeTableIndex < this->edges.size()){

        if(foundEdgeAB && foundEdgeBC && foundEdgeCA)
            break;

        if(!foundEdgeAB){
            if ((edges[edgeTableIndex].vStartId == vertexA or edges[edgeTableIndex].vStartId == vertexB) && (edges[edgeTableIndex].vEndId == vertexA or edges[edgeTableIndex].vEndId == vertexB)){
                edgeABID = edgeTableIndex;
                foundEdgeAB = true;
                openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeABID), openEdgesQueue.end());
            }
        }

        if(!foundEdgeBC){
            if ((edges[edgeTableIndex].vStartId == vertexB or edges[edgeTableIndex].vStartId == vertexC) && (edges[edgeTableIndex].vEndId == vertexB or edges[edgeTableIndex].vEndId == vertexC)){
                edgeBCID = edgeTableIndex;
                foundEdgeBC = true;
                openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeBCID), openEdgesQueue.end());
            }
        }

        if(!foundEdgeCA){
            if ((edges[edgeTableIndex].vStartId == vertexC or edges[edgeTableIndex].vStartId == vertexA) && (edges[edgeTableIndex].vEndId == vertexC or edges[edgeTableIndex].vEndId == vertexA)){
                edgeCAID = edgeTableIndex;
                foundEdgeCA = true;
                openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeCAID), openEdgesQueue.end());
            }
        }

        edgeTableIndex++;
    }

    if(!foundEdgeAB){
        edgeABID = edges.size();
        WingedEdge edgeAB = WingedEdge(edgeABID, vertexA, vertexB);
        edges.push_back(edgeAB);
        openEdgesQueue.push_back(edgeABID);
    }

    if(!foundEdgeBC){
        edgeBCID = edges.size();
        WingedEdge edgeBC = WingedEdge(edgeBCID, vertexB, vertexC);
        edges.push_back(edgeBC);
        openEdgesQueue.push_back(edgeBCID);
    }

    if(!foundEdgeCA){
        edgeCAID = edges.size();
        WingedEdge edgeCA = WingedEdge(edgeCAID, vertexC, vertexA);
        edges.push_back(edgeCA);
        openEdgesQueue.push_back(edgeCAID);
    }

    glm::vec3 faceNormal = glm::normalize(glm::cross(vertices[vertexB].position - vertices[vertexA].position, vertices[vertexC].position - vertices[vertexA].position));
    Face newFace = Face(faces.size(), edgeABID, faceNormal);// use edge AB as standard, can change if needed
    faces.push_back(newFace);

    if(edgeABID > edgesTableHighestID){
        edges[edgeABID].eLeftNextId = edgeBCID;
        edges[edgeABID].eLeftPrevId = edgeCAID;
        edges[edgeABID].fLeftId = newFace.id;
    }

    else{
        edges[edgeABID].eRightNextId = edgeCAID;
        edges[edgeABID].eRightPrevId = edgeBCID;
        edges[edgeABID].fRightId = newFace.id;
    }

    if(edgeBCID > edgesTableHighestID){
        edges[edgeBCID].eLeftNextId = edgeCAID;
        edges[edgeBCID].eLeftPrevId = edgeABID;
        edges[edgeBCID].fLeftId = newFace.id;
    }

    else{
        edges[edgeBCID].eRightNextId = edgeCAID;
        edges[edgeBCID].eRightPrevId = edgeBCID;
        edges[edgeBCID].fRightId = newFace.id;
    }

    if(edgeCAID > edgesTableHighestID){
        edges[edgeCAID].eLeftNextId = edgeABID;
        edges[edgeCAID].eLeftPrevId = edgeBCID;
        edges[edgeCAID].fLeftId = newFace.id;
    }

    else{
        edges[edgeCAID].eRightNextId = edgeCAID;
        edges[edgeCAID].eRightPrevId = edgeBCID;
        edges[edgeCAID].fRightId = newFace.id;
    }

    if (vertices[vertexA].edgeId == -1) vertices[vertexA].edgeId = edgeABID;
    if (vertices[vertexB].edgeId == -1) vertices[vertexB].edgeId = edgeBCID;
    if (vertices[vertexC].edgeId == -1) vertices[vertexC].edgeId = edgeCAID;

    return newFace.id;
}

std::vector<glm::vec3> WingedEdgeMesh::extractTriangleVertices() const {
    std::vector<glm::vec3> triangles;
    std::vector<int> vertexIds;

    for (const auto& face : faces) {
        int startEdgeId = face.edgeId;

        
        if (edges[startEdgeId].fLeftId == face.id){

            vertexIds.push_back(edges[startEdgeId].vStartId);
            vertexIds.push_back(edges[startEdgeId].vEndId);
            
            if (edges[edges[startEdgeId].eLeftNextId].vEndId == vertexIds[vertexIds.size()-1] || edges[edges[startEdgeId].eLeftNextId].vEndId == vertexIds[vertexIds.size()-1])
                vertexIds.push_back(edges[edges[startEdgeId].eLeftNextId].vStartId);
            else
                vertexIds.push_back(edges[edges[startEdgeId].eLeftNextId].vEndId);
        }

        else {

            vertexIds.push_back(edges[startEdgeId].vEndId);
            vertexIds.push_back(edges[startEdgeId].vStartId);

            if (edges[edges[startEdgeId].eRightNextId].vEndId == vertexIds[vertexIds.size()-1] || edges[edges[startEdgeId].eRightNextId].vEndId == vertexIds[vertexIds.size()-2])
                vertexIds.push_back(edges[edges[startEdgeId].eRightNextId].vStartId);
            else
                vertexIds.push_back(edges[edges[startEdgeId].eRightNextId].vEndId);
        }

        //add the last three points
        triangles.push_back(vertices[vertexIds[vertexIds.size()-3]].position);
        triangles.push_back(vertices[vertexIds[vertexIds.size()-2]].position);
        triangles.push_back(vertices[vertexIds[vertexIds.size()-1]].position);
    }

    return triangles;
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
        std::cout << "Face " << f.id << ", EdgeId: " << f.edgeId << ", faceNormal: (" 
                  << f.faceNormal.x << ", " << f.faceNormal.y << ", " << f.faceNormal.z << ") " <<'\n';
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
