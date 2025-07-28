#include "../headers/WingedEdgeMesh.h"

int WingedEdgeMesh::AddVertex(const glm::vec3& pos) {
    int id = static_cast<int>(vertices.size());
    vertices.push_back(Vertex{ id, -1, pos });
    return id;
}


int WingedEdgeMesh::CheckHowManyEdgesTriangleRemovesFromQueue(int vertexA, int vertexB, int vertexC){
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

void WingedEdgeMesh::AddFaceFromVertices(int vertexA, int vertexB, int vertexC) {
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
}


std::vector<glm::vec3> WingedEdgeMesh::ExtractVerticesPositions(){
    std::vector<glm::vec3> verticesCoordinates;

    for (int i = 0; i < vertices.size(); i++){
        verticesCoordinates.push_back(vertices[i].position);
    }

    return verticesCoordinates;
}

void WingedEdgeMesh::DeleteFace(int faceID){
    int mainEdgeID = faces[faceID].edgeId;
    int forwardEdgeID, backwardEdgeID;
    bool isLeftFaceMainEdge, isLeftFaceForwardEdge, isLeftFaceBackwardEdge;

    if(edges[mainEdgeID].fLeftId == faceID){
        forwardEdgeID = edges[mainEdgeID].eLeftNextId;
        backwardEdgeID = edges[mainEdgeID].eLeftPrevId;
        isLeftFaceMainEdge = true;
    }

    else {
        forwardEdgeID = edges[mainEdgeID].eRightNextId;
        backwardEdgeID = edges[mainEdgeID].eRightPrevId;
        isLeftFaceMainEdge = false;
    }

    if(edges[forwardEdgeID].fLeftId == faceID)  
        isLeftFaceForwardEdge = true;
    else
        isLeftFaceForwardEdge = false;

    if(edges[backwardEdgeID].fLeftId == faceID)  
        isLeftFaceBackwardEdge = true;
    else
        isLeftFaceBackwardEdge = false;

    DeleteFaceFromEdge(faceID, mainEdgeID, isLeftFaceMainEdge);
    DeleteFaceFromEdge(faceID, forwardEdgeID, isLeftFaceForwardEdge);
    DeleteFaceFromEdge(faceID, backwardEdgeID, isLeftFaceBackwardEdge);

    faces[faceID].edgeId = -1;
    faces[faceID].id = -1;
    faces[faceID].faceNormal = glm::vec3(0,0,0);
}

void WingedEdgeMesh::DeleteFaceFromEdge(int faceID, int edgeID, bool isLeftFace){
    if (isLeftFace){
        edges[edgeID].eLeftNextId = -1;
        edges[edgeID].eLeftPrevId = -1;
        edges[edgeID].fLeftId = -1;

        if(edges[edgeID].fRightId == -1){
            if(vertices[edges[edgeID].vEndId].edgeId == edgeID)
                vertices[edges[edgeID].vEndId].edgeId = -1;
            else if (vertices[edges[edgeID].vStartId].edgeId == edgeID)
                vertices[edges[edgeID].vStartId].edgeId = -1;
            
            edges[edgeID].vStartId = -1;
            edges[edgeID].vEndId = -1;
            edges[edgeID].id = -1;

            openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeID), openEdgesQueue.end());
        }

        else {
            openEdgesQueue.push_back(edgeID);
        }
    }

    else{
        edges[edgeID].eRightNextId = -1;
        edges[edgeID].eRightPrevId = -1;
        edges[edgeID].fRightId = -1;

        if(edges[edgeID].fLeftId == -1){
            if(vertices[edges[edgeID].vEndId].edgeId == edgeID)
                vertices[edges[edgeID].vEndId].edgeId = -1;
            else if (vertices[edges[edgeID].vStartId].edgeId == edgeID)
                vertices[edges[edgeID].vStartId].edgeId = -1;
            
            edges[edgeID].vStartId = -1;
            edges[edgeID].vEndId = -1;
            edges[edgeID].id = -1;

            openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeID), openEdgesQueue.end());
        }

        else {
            openEdgesQueue.push_back(edgeID);
        }
    }
}

void WingedEdgeMesh::FixMeshAfterDeletions(){
    std::vector<int> deletedFaces, deletedEdges;

    for (int faceId = 0; faceId < faces.size(); faceId++){
        if (faces[faceId].id == -1){
            deletedFaces.push_back(faceId);
        }
    }

    for (int edgeId = 0; edgeId < edges.size(); edgeId++){
        if (edges[edgeId].id == -1){
            deletedEdges.push_back(edgeId);
        }
    }

    int currentDeletedTopFace;
    for(int i = deletedFaces.size() - 1; i >=0; i--){
        currentDeletedTopFace = deletedFaces[i];

        faces.erase(faces.begin() + currentDeletedTopFace);

        for (int faceIndex = faces.size() - 1; faceIndex >= 0; faceIndex--){
            if(faces[faceIndex].id > currentDeletedTopFace){
                faces[faceIndex].id--;
            }
            else{
                break;
            }
        }

        for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++){
            if(edges[edgeIndex].fLeftId > currentDeletedTopFace){
                edges[edgeIndex].fLeftId--;
            }
            if(edges[edgeIndex].fRightId > currentDeletedTopFace){
                edges[edgeIndex].fRightId--;
            } 
        }

    }

    int currentDeletedTopEdge;
    for (int i = deletedEdges.size() - 1; i >= 0; i--){
        currentDeletedTopEdge = deletedEdges[i];

        edges.erase(edges.begin() + currentDeletedTopEdge);

        for (int edgeIndex = edges.size() - 1; edgeIndex >= 0; edgeIndex--){
            if(edges[edgeIndex].id > currentDeletedTopEdge){
                edges[edgeIndex].id--;
            }
            if(edges[edgeIndex].eLeftNextId > currentDeletedTopEdge){
                edges[edgeIndex].eLeftNextId--;
            }
            if(edges[edgeIndex].eRightNextId > currentDeletedTopEdge){
                edges[edgeIndex].eRightNextId--;
            }
            if(edges[edgeIndex].eLeftPrevId > currentDeletedTopEdge){
                edges[edgeIndex].eLeftPrevId--;
            }
            if(edges[edgeIndex].eRightPrevId > currentDeletedTopEdge){
                edges[edgeIndex].eRightPrevId--;
            }
        }

        for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++){
            if(faces[faceIndex].edgeId > currentDeletedTopEdge)
                faces[faceIndex].edgeId--;
        }

        for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++){
            if(vertices[vertexIndex].edgeId > currentDeletedTopEdge)
                vertices[vertexIndex].edgeId--;
        }

        for (int i = 0; i < openEdgesQueue.size(); i++){
            if(openEdgesQueue[i] > currentDeletedTopEdge)
                openEdgesQueue[i]--;
        }

    }

}

void WingedEdgeMesh::AppendDataToLinkHulls(int verticesAmount, int edgesAmount, int facesAmount){

    for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++){
            edges[edgeIndex].id += edgesAmount;
            edges[edgeIndex].eLeftNextId += edgesAmount;
            edges[edgeIndex].eLeftPrevId += edgesAmount;
            edges[edgeIndex].eRightNextId += edgesAmount;
            edges[edgeIndex].eRightPrevId += edgesAmount;
            edges[edgeIndex].vEndId += verticesAmount;
            edges[edgeIndex].vStartId += verticesAmount;
            edges[edgeIndex].fLeftId += facesAmount;
            edges[edgeIndex].fRightId += facesAmount;
        }

        for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++){
            faces[faceIndex].edgeId += edgesAmount;
            faces[faceIndex].id += facesAmount;
        }

        for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++){
            vertices[vertexIndex].edgeId += edgesAmount;
            vertices[vertexIndex].id += verticesAmount;
        }

        for (int i = 0; i < openEdgesQueue.size(); i++){
            openEdgesQueue[i]+=edgesAmount;
        }
}

void WingedEdgeMesh::AddFirstMergingEdge(){
    int lowestLeftHullVertexY, lowestRightHullVertexY, lowestLeftHullVertexYIndex, lowestRightHullVertexYIndex;
    int leftHullEdgeOrigin, rightHullEdgeOrigin;

    //CAN IMPROVE TO CHECK POINTS ONLY ONCE, DOING IT LAZY NOW AND CHECKING TWICE
    for(int i = 0; i < leftHullOpenEdgesQueue.size(); i++){
        if (i == 0){
            if (vertices[edges[leftHullOpenEdgesQueue[i]].vStartId].position.y < vertices[edges[leftHullOpenEdgesQueue[i]].vEndId].position.y){
                lowestLeftHullVertexY = vertices[edges[leftHullOpenEdgesQueue[i]].vStartId].position.y;
                lowestLeftHullVertexYIndex = edges[leftHullOpenEdgesQueue[i]].vStartId;
                leftHullEdgeOrigin = leftHullOpenEdgesQueue[i];
            }

            else{
                lowestLeftHullVertexY = vertices[edges[leftHullOpenEdgesQueue[i]].vEndId].position.y;
                lowestLeftHullVertexYIndex = edges[leftHullOpenEdgesQueue[i]].vEndId;
                leftHullEdgeOrigin = leftHullOpenEdgesQueue[i];
            }
        }

        else{
            if (vertices[edges[leftHullOpenEdgesQueue[i]].vStartId].position.y < lowestLeftHullVertexY){
                lowestLeftHullVertexY = vertices[edges[leftHullOpenEdgesQueue[i]].vStartId].position.y;
                lowestLeftHullVertexYIndex = edges[leftHullOpenEdgesQueue[i]].vStartId;
                leftHullEdgeOrigin = leftHullOpenEdgesQueue[i];
            }

            if (vertices[edges[leftHullOpenEdgesQueue[i]].vEndId].position.y < lowestLeftHullVertexY){
                lowestLeftHullVertexY = vertices[edges[leftHullOpenEdgesQueue[i]].vEndId].position.y;
                lowestLeftHullVertexYIndex = edges[leftHullOpenEdgesQueue[i]].vEndId;
                leftHullEdgeOrigin = leftHullOpenEdgesQueue[i];
            }
        }
    }

    for(int i = 0; i < rightHullOpenEdgesQueue.size(); i++){
        if (i == 0){
            if (vertices[edges[rightHullOpenEdgesQueue[i]].vStartId].position.y < vertices[edges[rightHullOpenEdgesQueue[i]].vEndId].position.y){
                lowestRightHullVertexY = vertices[edges[rightHullOpenEdgesQueue[i]].vStartId].position.y;
                lowestRightHullVertexYIndex = edges[rightHullOpenEdgesQueue[i]].vStartId;
                rightHullEdgeOrigin = rightHullOpenEdgesQueue[i];
            }

            else{
                lowestRightHullVertexY = vertices[edges[rightHullOpenEdgesQueue[i]].vEndId].position.y;
                lowestRightHullVertexYIndex = edges[rightHullOpenEdgesQueue[i]].vEndId;
                rightHullEdgeOrigin = rightHullOpenEdgesQueue[i];
            }
        }

        else{
            if (vertices[edges[rightHullOpenEdgesQueue[i]].vStartId].position.y < lowestLeftHullVertexY){
                lowestRightHullVertexY = vertices[edges[rightHullOpenEdgesQueue[i]].vStartId].position.y;
                lowestRightHullVertexYIndex = edges[rightHullOpenEdgesQueue[i]].vStartId;
                rightHullEdgeOrigin = rightHullOpenEdgesQueue[i];
            }

            if (vertices[edges[rightHullOpenEdgesQueue[i]].vEndId].position.y < lowestLeftHullVertexY){
                lowestRightHullVertexY = vertices[edges[rightHullOpenEdgesQueue[i]].vEndId].position.y;
                lowestRightHullVertexYIndex = edges[rightHullOpenEdgesQueue[i]].vEndId;
                rightHullEdgeOrigin = rightHullOpenEdgesQueue[i];
            }
        }
    }

    WingedEdge firstEdge(edges.size(), lowestLeftHullVertexYIndex, lowestRightHullVertexYIndex);
    openEdgesQueue.push_back(edges.size());
    edges.push_back(firstEdge);


}


void WingedEdgeMesh::OrderLeftAndRightQueues(int startVertexLeftId, int StartVertexRightId, int startEdgeLeftId, int startEdgeRightId){
    std::vector<int> orderedCWLeftHull, orderedCCWRightHull;

    if(edges[startEdgeLeftId].vEndId == startVertexLeftId){
        if (edges[startEdgeLeftId].fLeftId == -1){
            orderedCWLeftHull.push_back(startEdgeLeftId);
        }
        else{
            
        }
    }
}

std::vector<glm::vec3> WingedEdgeMesh::ExtractTriangleVertices() const {
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



void WingedEdgeMesh::DebugPrint() const {
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

void WingedEdgeMesh::Clear() {
    vertices.clear();
    edges.clear();
    faces.clear();
}
