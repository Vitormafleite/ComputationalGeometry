#include "../headers/WingedEdgeMesh.h"
#include <vector>

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
        if(edges[edgeABID].fRightId == -1){
            edges[edgeABID].eRightNextId = edgeCAID;
            edges[edgeABID].eRightPrevId = edgeBCID;
            edges[edgeABID].fRightId = newFace.id;

            if(edges[edgeABID].fLeftId != -1){
                glm::vec3 leftFaceNormal = faces[edges[edgeABID].fLeftId].faceNormal;
                glm::vec3 rightFaceNormal = faces[edges[edgeABID].fRightId].faceNormal;
                edges[edgeABID].edgeInstantNormal = glm::normalize(leftFaceNormal + rightFaceNormal);
            }
        }
        else if(edges[edgeABID].fLeftId == -1){
            edges[edgeABID].eLeftNextId = edgeBCID;
            edges[edgeABID].eLeftPrevId = edgeCAID;
            edges[edgeABID].fLeftId = newFace.id;

            if(edges[edgeABID].fRightId != -1){
                glm::vec3 leftFaceNormal = faces[edges[edgeABID].fLeftId].faceNormal;
                glm::vec3 rightFaceNormal = faces[edges[edgeABID].fRightId].faceNormal;
                edges[edgeABID].edgeInstantNormal = glm::normalize(leftFaceNormal + rightFaceNormal);
            }
        }
    }

    if(edgeBCID > edgesTableHighestID){
        edges[edgeBCID].eLeftNextId = edgeCAID;
        edges[edgeBCID].eLeftPrevId = edgeABID;
        edges[edgeBCID].fLeftId = newFace.id;
    }

    else{
        if(edges[edgeBCID].fRightId == -1){
            edges[edgeBCID].eRightNextId = edgeABID;
            edges[edgeBCID].eRightPrevId = edgeCAID;
            edges[edgeBCID].fRightId = newFace.id;

            if(edges[edgeBCID].fLeftId != -1){
                glm::vec3 leftFaceNormal = faces[edges[edgeBCID].fLeftId].faceNormal;
                glm::vec3 rightFaceNormal = faces[edges[edgeBCID].fRightId].faceNormal;
                edges[edgeBCID].edgeInstantNormal = glm::normalize(leftFaceNormal + rightFaceNormal);
            }
        }
        else if(edges[edgeBCID].fLeftId == -1){
            edges[edgeBCID].eLeftNextId = edgeCAID;
            edges[edgeBCID].eLeftPrevId = edgeABID;
            edges[edgeBCID].fLeftId = newFace.id;

            if(edges[edgeBCID].fRightId != -1){
                glm::vec3 leftFaceNormal = faces[edges[edgeBCID].fLeftId].faceNormal;
                glm::vec3 rightFaceNormal = faces[edges[edgeBCID].fRightId].faceNormal;
                edges[edgeBCID].edgeInstantNormal = glm::normalize(leftFaceNormal + rightFaceNormal);
            }
        }
    }

    if(edgeCAID > edgesTableHighestID){
        edges[edgeCAID].eLeftNextId = edgeABID;
        edges[edgeCAID].eLeftPrevId = edgeBCID;
        edges[edgeCAID].fLeftId = newFace.id;
    }

    else{
        if(edges[edgeCAID].fRightId == -1){
            edges[edgeCAID].eRightNextId = edgeBCID;
            edges[edgeCAID].eRightPrevId = edgeABID;
            edges[edgeCAID].fRightId = newFace.id;

            if(edges[edgeCAID].fLeftId != -1){
                glm::vec3 leftFaceNormal = faces[edges[edgeCAID].fLeftId].faceNormal;
                glm::vec3 rightFaceNormal = faces[edges[edgeCAID].fRightId].faceNormal;
                edges[edgeCAID].edgeInstantNormal = glm::normalize(leftFaceNormal + rightFaceNormal);
            }
        }
        else if (edges[edgeCAID].fLeftId == -1) {
            edges[edgeCAID].eLeftNextId = edgeABID;
            edges[edgeCAID].eLeftPrevId = edgeBCID;
            edges[edgeCAID].fLeftId = newFace.id;

            if(edges[edgeCAID].fRightId != -1){
                glm::vec3 leftFaceNormal = faces[edges[edgeCAID].fLeftId].faceNormal;
                glm::vec3 rightFaceNormal = faces[edges[edgeCAID].fRightId].faceNormal;
                edges[edgeCAID].edgeInstantNormal = glm::normalize(leftFaceNormal + rightFaceNormal);
            }
        }
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

bool WingedEdgeMesh::EdgeSeesOtherHull(glm::vec3 edgeNormal, glm::vec3 edgeVertex, std::vector<glm::vec3> otherHullVertices){
    const float epsilon = 1e-6f;
    float dot;
    int pointsInFront = 0;

    for (const auto& vertex : otherHullVertices){
        dot = glm::dot(vertex - edgeVertex, edgeNormal);
        if(dot > epsilon){
            pointsInFront++;
            if (pointsInFront >= 2) {
                return true;
            }
        }
    }

    return false;
}

void WingedEdgeMesh::DeleteFace(int faceID, std::vector<glm::vec3> otherHullcloudPoints){
    int mainEdgeID = faces[faceID].edgeId;
    int forwardEdgeID, backwardEdgeID;
    bool isLeftFaceMainEdge, isLeftFaceForwardEdge, isLeftFaceBackwardEdge;

    bool mainEdgeNotPersistant, forwardEdgeNotPersistant, backwardEdgeNotPersistant;

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


    //TEST EDGES PERSISTANCE
    mainEdgeNotPersistant = EdgeSeesOtherHull(edges[mainEdgeID].edgeInstantNormal, vertices[edges[mainEdgeID].vStartId].position, otherHullcloudPoints);
    forwardEdgeNotPersistant = EdgeSeesOtherHull(edges[forwardEdgeID].edgeInstantNormal, vertices[edges[forwardEdgeID].vStartId].position, otherHullcloudPoints);
    backwardEdgeNotPersistant = EdgeSeesOtherHull(edges[backwardEdgeID].edgeInstantNormal, vertices[edges[backwardEdgeID].vStartId].position, otherHullcloudPoints);

    if(mainEdgeID == 1){
        std::cout << "Edge 1 is not persistent: " << mainEdgeNotPersistant << std::endl;
    }

    if(forwardEdgeID == 1){
        std::cout << "Edge 1 is not persistent: " << forwardEdgeNotPersistant << std::endl;
    }

    if(backwardEdgeID == 1){
        std::cout << "Edge 1 is not persistent: " << backwardEdgeNotPersistant << std::endl;
    }

    DeleteFaceFromEdge(faceID, mainEdgeID, isLeftFaceMainEdge, mainEdgeNotPersistant);
    DeleteFaceFromEdge(faceID, forwardEdgeID, isLeftFaceForwardEdge, forwardEdgeNotPersistant);
    DeleteFaceFromEdge(faceID, backwardEdgeID, isLeftFaceBackwardEdge, backwardEdgeNotPersistant);

    faces[faceID].edgeId = -1;
    faces[faceID].id = -1;
    faces[faceID].faceNormal = glm::vec3(0,0,0);
}

void WingedEdgeMesh::DeleteFaceFromEdge(int faceID, int edgeID, bool isLeftFace, bool edgeIsNotPersistant){
    if (isLeftFace){
        edges[edgeID].eLeftNextId = -1;
        edges[edgeID].eLeftPrevId = -1;
        edges[edgeID].fLeftId = -1;

        if(edges[edgeID].fRightId == -1){
            if(vertices[edges[edgeID].vEndId].edgeId == edgeID)
                vertices[edges[edgeID].vEndId].edgeId = -1;
            else if (vertices[edges[edgeID].vStartId].edgeId == edgeID)
                vertices[edges[edgeID].vStartId].edgeId = -1;
            
            if(edgeIsNotPersistant){
                edges[edgeID].vStartId = -1;
                edges[edgeID].vEndId = -1;
                edges[edgeID].id = -1;

                openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeID), openEdgesQueue.end());
            }

            else{//edge is persistant
                std::cout << " FOUND PERSISTANT EDGE1" << std::endl;
                std::cout << " Edge Id: " << edgeID << std::endl;
                openEdgesQueue.push_back(edgeID);
            }
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
            

            if(edgeIsNotPersistant){
                edges[edgeID].vStartId = -1;
                edges[edgeID].vEndId = -1;
                edges[edgeID].id = -1;

                openEdgesQueue.erase(std::remove(openEdgesQueue.begin(), openEdgesQueue.end(), edgeID), openEdgesQueue.end());
            }

            else{//edge is persistant
                std::cout << " FOUND PERSISTANT EDGE2" << std::endl;
                std::cout << " Edge Id: " << edgeID << std::endl;
                openEdgesQueue.push_back(edgeID);
            }

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
            if(edges[edgeIndex].fLeftId != -1){
                edges[edgeIndex].eLeftNextId += edgesAmount;
                edges[edgeIndex].eLeftPrevId += edgesAmount;
                edges[edgeIndex].fLeftId += facesAmount;
            }

            if(edges[edgeIndex].fRightId != -1){
                edges[edgeIndex].eRightNextId += edgesAmount;
                edges[edgeIndex].eRightPrevId += edgesAmount;
                edges[edgeIndex].fRightId += facesAmount;
            }

            edges[edgeIndex].vEndId += verticesAmount;
            edges[edgeIndex].vStartId += verticesAmount;
            
        }

        for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++){
            faces[faceIndex].edgeId += edgesAmount;
            faces[faceIndex].id += facesAmount;
        }

        for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++){
            if(vertices[vertexIndex].edgeId != -1){
                vertices[vertexIndex].edgeId += edgesAmount;
            }
            vertices[vertexIndex].id += verticesAmount;
        }

        for (int i = 0; i < openEdgesQueue.size(); i++){
            openEdgesQueue[i]+=edgesAmount;
        }
}

void WingedEdgeMesh::DeleteEdgeFromLeftHullOpenEdgesQueue(int edgeId){
    for (auto it = leftHullOpenEdgesQueue.rbegin(); it != leftHullOpenEdgesQueue.rend(); ++it) {
        if (*it == edgeId) {
            leftHullOpenEdgesQueue.erase(std::next(it).base());
            break;
        }
    }
}

void WingedEdgeMesh::DeleteEdgeFromRightHullOpenEdgesQueue(int edgeId){
    for (auto it = rightHullOpenEdgesQueue.rbegin(); it != rightHullOpenEdgesQueue.rend(); ++it) {
        if (*it == edgeId) {
            rightHullOpenEdgesQueue.erase(std::next(it).base());
            break;
        }
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

    //DebugPrint();
//
    //std::cout<< "lowestLeftHullVertexYIndex: " << lowestLeftHullVertexYIndex <<std::endl;
    //std::cout<< "lowestRightHullVertexYIndex: " << lowestRightHullVertexYIndex <<std::endl;
    //std::cout<< "leftHullEdgeOrigin: " << leftHullEdgeOrigin <<std::endl;
    //std::cout<< "righttHullEdgeOrigin: " << rightHullEdgeOrigin <<std::endl;


    OrderLeftAndRightQueues(lowestLeftHullVertexYIndex, lowestRightHullVertexYIndex, leftHullEdgeOrigin, rightHullEdgeOrigin);

    //DebugPrint();

    //std::cout<< "left edges :" <<std::endl;
    //for (int i = 0; i < leftHullOpenEdgesQueue.size(); i++){
    //    std::cout<< leftHullOpenEdgesQueue[i] <<std::endl;
    //}
    //std::cout<< "right edges :" <<std::endl;
    //for (int i = 0; i < rightHullOpenEdgesQueue.size(); i++){
    //    std::cout<< rightHullOpenEdgesQueue[i] <<std::endl;
    //}

}

void WingedEdgeMesh::OrderLeftAndRightQueues(int startVertexLeftId, int startVertexRightId, int startEdgeLeftId, int startEdgeRightId){

//    std::cout << "Left before:" << std::endl;
//    for (int i = 0 ; i < leftHullOpenEdgesQueue.size(); i++){
//        std::cout << leftHullOpenEdgesQueue[i] << " " << std::endl;
//    }
//    std::cout << "Right before:" << std::endl;
//    for (int i = 0 ; i < rightHullOpenEdgesQueue.size(); i++){
//        std::cout << rightHullOpenEdgesQueue[i] << " " << std::endl;
//    }

    std::vector<int> orderedCWLeftHull, orderedCCWRightHull;

    int nextVertexToLookForLeft, nextVertexToLookForRight;

    //std::cout << "nextVertexToLookForLeft: " << nextVertexToLookForLeft << std::endl;
    //std::cout << "nextVertexToLookForRight: " << nextVertexToLookForRight << std::endl;
    //std::cout << "startEdgeLeftId: " << startEdgeLeftId << std::endl;
    //std::cout << "startEdgeRightId: " << startEdgeRightId << std::endl;
    //std::cout << "RightVertex: " << startVertexRightId << std::endl;
    //std::cout << "Looking for vertex: " << nextVertexToLookForRight << std::endl;

    if(edges[startEdgeLeftId].fLeftId == -1){
        if (edges[startEdgeLeftId].vEndId == startVertexLeftId){
            orderedCWLeftHull.push_back(startEdgeLeftId);
            //leftHullOpenEdgesQueue.erase(std::remove(leftHullOpenEdgesQueue.begin(), leftHullOpenEdgesQueue.end(), startEdgeLeftId), leftHullOpenEdgesQueue.end());
            DeleteEdgeFromLeftHullOpenEdgesQueue(startEdgeLeftId);
            nextVertexToLookForLeft = edges[startEdgeLeftId].vStartId;
        }
        else if (edges[startEdgeLeftId].vStartId == startVertexLeftId){
            for (int i = 0; i < leftHullOpenEdgesQueue.size(); i++){
                if (leftHullOpenEdgesQueue[i] != startEdgeLeftId){
                    if(edges[leftHullOpenEdgesQueue[i]].vEndId == startVertexLeftId && (edges[leftHullOpenEdgesQueue[i]].fRightId == -1 || edges[leftHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeLeftId = leftHullOpenEdgesQueue[i];
                        orderedCWLeftHull.push_back(startEdgeLeftId);
                        nextVertexToLookForLeft = edges[startEdgeLeftId].vStartId;
                        DeleteEdgeFromLeftHullOpenEdgesQueue(startEdgeLeftId);  
                        break;
                    }

                    else if(edges[i].vStartId == startVertexLeftId && (edges[i].fRightId == -1 || edges[i].fLeftId == -1)){
                        startEdgeLeftId = i;
                        orderedCWLeftHull.push_back(startEdgeLeftId);
                        DeleteEdgeFromLeftHullOpenEdgesQueue(startEdgeLeftId);
                        nextVertexToLookForLeft = edges[i].vEndId;
                        break;
                    }
                }
            }
        }
    }

    else if(edges[startEdgeLeftId].fRightId == -1){
        if (edges[startEdgeLeftId].vStartId == startVertexLeftId){
            orderedCWLeftHull.push_back(startEdgeLeftId);
            DeleteEdgeFromLeftHullOpenEdgesQueue(startEdgeLeftId);
            nextVertexToLookForLeft = edges[startEdgeLeftId].vEndId;
        }
        else if (edges[startEdgeLeftId].vEndId == startVertexLeftId){
            for (int i = 0; i < leftHullOpenEdgesQueue.size(); i++){
                if (leftHullOpenEdgesQueue[i] != startEdgeLeftId){
                    if(edges[leftHullOpenEdgesQueue[i]].vEndId == startVertexLeftId && (edges[leftHullOpenEdgesQueue[i]].fRightId == -1 || edges[leftHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeLeftId = leftHullOpenEdgesQueue[i];
                        orderedCWLeftHull.push_back(startEdgeLeftId);
                        nextVertexToLookForLeft = edges[startEdgeLeftId].vStartId;
                        DeleteEdgeFromLeftHullOpenEdgesQueue(startEdgeLeftId);  
                        break;
                    }

                    else if(edges[leftHullOpenEdgesQueue[i]].vStartId == startVertexLeftId && (edges[leftHullOpenEdgesQueue[i]].fRightId == -1 || edges[leftHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeLeftId = leftHullOpenEdgesQueue[i];
                        orderedCWLeftHull.push_back(startEdgeLeftId);
                        nextVertexToLookForLeft = edges[startEdgeLeftId].vEndId;
                        DeleteEdgeFromLeftHullOpenEdgesQueue(startEdgeLeftId);
                        break;
                    }
                }
            }
        }
    }

    if(edges[startEdgeRightId].fLeftId == -1){
        if (edges[startEdgeRightId].vStartId == startVertexRightId){
            orderedCCWRightHull.push_back(startEdgeRightId);
            //rightHullOpenEdgesQueue.erase(std::remove(rightHullOpenEdgesQueue.begin(), rightHullOpenEdgesQueue.end(), startEdgeRightId), rightHullOpenEdgesQueue.end());
            DeleteEdgeFromRightHullOpenEdgesQueue(startEdgeRightId);
            nextVertexToLookForRight = edges[startEdgeRightId].vEndId; 
        }
        else if (edges[startEdgeRightId].vEndId == startVertexRightId){
            for (int i = 0; i < rightHullOpenEdgesQueue.size(); i++){
                if (rightHullOpenEdgesQueue[i] != startEdgeRightId){
                    if(edges[rightHullOpenEdgesQueue[i]].vEndId == startVertexRightId && (edges[rightHullOpenEdgesQueue[i]].fRightId == -1 || edges[rightHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeRightId = rightHullOpenEdgesQueue[i];
                        orderedCCWRightHull.push_back(startEdgeRightId);
                        nextVertexToLookForRight = edges[startEdgeRightId].vStartId; 
                        DeleteEdgeFromRightHullOpenEdgesQueue(startEdgeRightId);  
                        break;
                    }

                    else if(edges[rightHullOpenEdgesQueue[i]].vStartId == startVertexRightId && (edges[rightHullOpenEdgesQueue[i]].fRightId == -1 || edges[rightHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeRightId = rightHullOpenEdgesQueue[i];
                        orderedCCWRightHull.push_back(startEdgeRightId);
                        nextVertexToLookForRight = edges[startEdgeRightId].vEndId;
                        DeleteEdgeFromRightHullOpenEdgesQueue(startEdgeRightId);
                        break;
                    }
                }
            }
        }
    }

    else if(edges[startEdgeRightId].fRightId == -1){
        if (edges[startEdgeRightId].vEndId == startVertexRightId){
            orderedCCWRightHull.push_back(startEdgeRightId);
            DeleteEdgeFromRightHullOpenEdgesQueue(startEdgeRightId);
            nextVertexToLookForRight = edges[startEdgeRightId].vStartId; 
        }
        else if (edges[startEdgeRightId].vStartId == startVertexRightId){
            for (int i = 0; i < rightHullOpenEdgesQueue.size(); i++){
                if (rightHullOpenEdgesQueue[i] != startEdgeRightId){
                    if(edges[rightHullOpenEdgesQueue[i]].vEndId == startVertexRightId && (edges[rightHullOpenEdgesQueue[i]].fRightId == -1 || edges[rightHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeRightId = rightHullOpenEdgesQueue[i];
                        orderedCCWRightHull.push_back(startEdgeRightId);
                        nextVertexToLookForRight = edges[startEdgeRightId].vStartId;
                        DeleteEdgeFromRightHullOpenEdgesQueue(startEdgeRightId);  
                        break;
                    }

                    else if(edges[rightHullOpenEdgesQueue[i]].vStartId == startVertexRightId && (edges[rightHullOpenEdgesQueue[i]].fRightId == -1 || edges[rightHullOpenEdgesQueue[i]].fLeftId == -1)){
                        startEdgeRightId = rightHullOpenEdgesQueue[i];
                        orderedCCWRightHull.push_back(startEdgeRightId);
                        nextVertexToLookForRight = edges[startEdgeRightId].vEndId;
                        DeleteEdgeFromRightHullOpenEdgesQueue(startEdgeRightId);
                        break;
                    }
                }
            }
        }
    }

    int lastIndex = -1;

    while (leftHullOpenEdgesQueue.size() > 0){
        
        if(lastIndex != leftHullOpenEdgesQueue.size()){
            lastIndex = leftHullOpenEdgesQueue.size();
            std::cout << " leftHullOpenEdgesQueue:" << std::endl;
            for (int i = 0 ; i < leftHullOpenEdgesQueue.size(); i++){
                std::cout << leftHullOpenEdgesQueue[i] << " " << std::endl;
            }
        }       

        for (int i = 0; i < leftHullOpenEdgesQueue.size(); i++){
            if(edges[leftHullOpenEdgesQueue[i]].vEndId == nextVertexToLookForLeft){
                orderedCWLeftHull.push_back(leftHullOpenEdgesQueue[i]);
                nextVertexToLookForLeft = edges[leftHullOpenEdgesQueue[i]].vStartId;
                DeleteEdgeFromLeftHullOpenEdgesQueue(leftHullOpenEdgesQueue[i]);
            }

            else if(edges[leftHullOpenEdgesQueue[i]].vStartId == nextVertexToLookForLeft){
                orderedCWLeftHull.push_back(leftHullOpenEdgesQueue[i]);
                nextVertexToLookForLeft = edges[leftHullOpenEdgesQueue[i]].vEndId;
                DeleteEdgeFromLeftHullOpenEdgesQueue(leftHullOpenEdgesQueue[i]);
            }
        }
    }


    while (rightHullOpenEdgesQueue.size() > 0){
        for (int i = 0; i < rightHullOpenEdgesQueue.size(); i++){
            if(edges[rightHullOpenEdgesQueue[i]].vEndId == nextVertexToLookForRight){
                orderedCCWRightHull.push_back(rightHullOpenEdgesQueue[i]);
                nextVertexToLookForRight = edges[rightHullOpenEdgesQueue[i]].vStartId;
                DeleteEdgeFromRightHullOpenEdgesQueue(rightHullOpenEdgesQueue[i]);
            }

            else if(edges[rightHullOpenEdgesQueue[i]].vStartId == nextVertexToLookForRight){
                orderedCCWRightHull.push_back(rightHullOpenEdgesQueue[i]);
                nextVertexToLookForRight = edges[rightHullOpenEdgesQueue[i]].vEndId;
                DeleteEdgeFromRightHullOpenEdgesQueue(rightHullOpenEdgesQueue[i]);
            } 
        }
    }

    std::reverse(orderedCCWRightHull.begin(), orderedCCWRightHull.end());
    std::reverse(orderedCWLeftHull.begin(), orderedCWLeftHull.end());
    
    rightHullOpenEdgesQueue = orderedCCWRightHull;
    leftHullOpenEdgesQueue = orderedCWLeftHull;

    DebugPrint();

    std::cout << " leftHullOpenEdgesQueue after:" << std::endl;
    for (int i = 0 ; i < leftHullOpenEdgesQueue.size(); i++){
        std::cout << leftHullOpenEdgesQueue[i] << " " << std::endl;
    }

    std::cout << " rightHullOpenEdgesQueue after:" << std::endl;
    for (int i = 0 ; i < rightHullOpenEdgesQueue.size(); i++){
        std::cout << rightHullOpenEdgesQueue[i] << " " << std::endl;
    }

}

void WingedEdgeMesh::SewHulls(){
    const float epsilon = 1e-6f;
    bool isFirstIteration = true;
    int vertex1, vertex2, aux;

    int nextEdgeLeft, nextEdgeRight;
    int nextVertexLeft, nextVertexRight;    

    int firtsSewedEdge = openEdgesQueue.back();

    int maxSewings = 9;
    int sewings = 0;

    while(openEdgesQueue.size() > 0 ){

        vertex1 = edges[openEdgesQueue.back()].vStartId;
        vertex2 = edges[openEdgesQueue.back()].vEndId;

        if (vertex1 > vertex2){
            aux = vertex1;
            vertex1 = vertex2;
            vertex2 = aux;
        }


        if(rightHullOpenEdgesQueue.size() > 0 && leftHullOpenEdgesQueue.size() > 0){
            nextEdgeLeft = leftHullOpenEdgesQueue.back();
            if (edges[nextEdgeLeft].vEndId == vertex1){
                nextVertexLeft = edges[nextEdgeLeft].vStartId;
            }
            else if (edges[nextEdgeLeft].vStartId == vertex1){
                nextVertexLeft = edges[nextEdgeLeft].vEndId;
            }
    
            nextEdgeRight = rightHullOpenEdgesQueue.back();
            if (edges[nextEdgeRight].vEndId == vertex2){
                nextVertexRight = edges[nextEdgeRight].vStartId;
            }
            else if (edges[nextEdgeRight].vStartId == vertex2){
                nextVertexRight = edges[nextEdgeRight].vEndId;
            }

            glm::vec3 A = vertices[vertex1].position;
            glm::vec3 B = vertices[vertex2].position;
            glm::vec3 C = vertices[nextVertexLeft].position;
            
            glm::vec3 P = vertices[nextVertexRight].position;
            
            glm::vec3 faceNormal = glm::normalize(glm::cross(B - A, C - A));
            
            float dotProduct = glm::dot(P - A, faceNormal);
            
            bool nextVertexRightInFront = dotProduct > epsilon;
            bool nextVertexRightBehind = dotProduct < -epsilon;
            bool nextVertexRightOnFace = !nextVertexRightInFront && !nextVertexRightBehind;

            if(isFirstIteration){
                edges.pop_back();
                isFirstIteration = false;
            }

            if (nextVertexRightBehind) {
                AddFaceFromVertices(vertex1, vertex2, nextVertexLeft);
                leftHullOpenEdgesQueue.pop_back();

            } else if (nextVertexRightInFront) {
                AddFaceFromVertices(vertex1, vertex2, nextVertexRight);
                rightHullOpenEdgesQueue.pop_back();

            } else {
                //coplanar case, just go with left side
                AddFaceFromVertices(vertex1, vertex2, nextVertexLeft);
                leftHullOpenEdgesQueue.pop_back();
            }
        }

        else if(leftHullOpenEdgesQueue.size() > 0){
            nextEdgeLeft = leftHullOpenEdgesQueue.back();
            if (edges[nextEdgeLeft].vEndId == vertex1){
                nextVertexLeft = edges[nextEdgeLeft].vStartId;
            }
            else if (edges[nextEdgeLeft].vStartId == vertex1){
                nextVertexLeft = edges[nextEdgeLeft].vEndId;
            }

            AddFaceFromVertices(vertex1, vertex2, nextVertexLeft);
            leftHullOpenEdgesQueue.pop_back();
        }

        else if (rightHullOpenEdgesQueue.size() > 0){
            nextEdgeRight = rightHullOpenEdgesQueue.back();
            if (edges[nextEdgeRight].vEndId == vertex2){
                nextVertexRight = edges[nextEdgeRight].vStartId;
            }
            else if (edges[nextEdgeRight].vStartId == vertex2){
                nextVertexRight = edges[nextEdgeRight].vEndId;
            }

            AddFaceFromVertices(vertex1, vertex2, nextVertexRight);
            rightHullOpenEdgesQueue.pop_back();
        } 
        
        sewings++;

        //std::cout<< "left edges :" <<std::endl;
        //for (int i = 0; i < leftHullOpenEdgesQueue.size(); i++){
        //    std::cout<< leftHullOpenEdgesQueue[i] <<std::endl;
        //}
        //std::cout<< "right edges :" <<std::endl;
        //for (int i = 0; i < rightHullOpenEdgesQueue.size(); i++){
        //    std::cout<< rightHullOpenEdgesQueue[i] <<std::endl;
        //}
        
    }
    //DebugPrint();
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

        std::cout << ", EdgeInstantNormal " << ": ("
        << e.edgeInstantNormal.x << ", " << e.edgeInstantNormal.y << ", " << e.edgeInstantNormal.z
        << ")" << '\n';
    }
    std::cout << "=====================\n";
}

void WingedEdgeMesh::Clear() {
    vertices.clear();
    edges.clear();
    faces.clear();
}
