#include "../headers/Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

Mesh::Mesh(){}

const std::vector<Vector3>& Mesh::getVertices() const { return vertices; }


const std::vector<unsigned int>& Mesh::getIndices() const { return indices; }

void Mesh::ClearMeshData(){
    vertices.clear(); 
    submeshesVertices.clear();
    partitions.clear();
    localHulls.clear();
    mergeHullPartitionsColections.clear();
    indices.clear();
};

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
            //hullsForSubmesh.push_back(buildInitialHull(group));
            hullsForSubmesh.push_back(InitialHull(group));
        }
        localHulls.push_back(std::move(hullsForSubmesh));
    }

    mergeHullPartitionsColections.push_back(localHulls);
}


//THIS HWOLE FUNCTION CAN BE IMPROVED WITH MORE TIME 
WingedEdgeMesh Mesh::InitialHull(const std::vector<Vector3>& group) {

    WingedEdgeMesh hull;
    std::vector<int> vIds;

    const float epsilon = 1e-6f;

    for (const auto& point : group) {
        //converting to glm because why not
        glm::vec3 glmPoint(point.x, point.y, point.z);
        vIds.push_back(hull.AddVertex(glmPoint));
    }

    if (group.size() == 4) {
        //AGAIN PLEASE REFACTOR THIS - CREATE A NEW FUNCTION TO TEST POINTS AGAINST TRIANGLE 
        glm::vec3 pointA(group[1].x, group[1].y, group[1].z);
        glm::vec3 pointB(group[2].x, group[2].y, group[2].z);
        glm::vec3 pointC(group[3].x, group[3].y, group[3].z);
        glm::vec3 triangleNormal = glm::normalize(glm::cross(pointB - pointA, pointC - pointA));

        glm::vec3 pointToCheckFaceOrientation(group[0].x, group[0].y, group[0].z);
        float d = glm::dot(pointToCheckFaceOrientation - pointA, triangleNormal);

        //with four points we can just trivialy the triangles
        if (d > epsilon) {
            hull.AddFaceFromVertices(vIds[1], vIds[3], vIds[2]);
            hull.AddFaceFromVertices(vIds[1], vIds[2], vIds[0]);
            hull.AddFaceFromVertices(vIds[3], vIds[1], vIds[0]);
            hull.AddFaceFromVertices(vIds[2], vIds[3], vIds[0]);
            
        } else {
            hull.AddFaceFromVertices(vIds[1], vIds[2], vIds[3]);
            hull.AddFaceFromVertices(vIds[2], vIds[1], vIds[0]);
            hull.AddFaceFromVertices(vIds[1], vIds[3], vIds[0]);
            hull.AddFaceFromVertices(vIds[3], vIds[2], vIds[0]);
        }

    }

    //ONLY WORKS FOR CASE 5, NEED TO FIX FOR CASE 6 - 7 - 11
    else if (group.size() == 5) {
        bool foundValidFace = false;
        for (int i = 0; i < 5 && !foundValidFace; ++i) {
            for (int j = i + 1; j < 5 && !foundValidFace; ++j) {
                for (int k = j + 1; k < 5 && !foundValidFace; ++k) {
                    glm::vec3 A(group[i].x, group[i].y, group[i].z);
                    glm::vec3 B(group[j].x, group[j].y, group[j].z);
                    glm::vec3 C(group[k].x, group[k].y, group[k].z);

                    // Calculate face normal
                    glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
                    if (glm::length(normal) < epsilon) continue;

                    // Test remaining points
                    float dot1 = -1, dot2 = -1;
                    int remainingIdx1 = -1, remainingIdx2 = -1;
                    int currentRem = 0;

                    // Find the two remaining points and test them
                    for (int l = 0; l < 5; ++l) {
                        if (l != i && l != j && l != k) {
                            glm::vec3 P(group[l].x, group[l].y, group[l].z);
                            if (currentRem == 0) {
                                dot1 = glm::dot(P - A, normal);
                                remainingIdx1 = l;
                            } else {
                                dot2 = glm::dot(P - A, normal);
                                remainingIdx2 = l;
                            }
                            currentRem++;
                        }
                    }

                    // Check if remaining points are on the same side
                    if ((dot1 > epsilon && dot2 > epsilon) || (dot1 < -epsilon && dot2 < -epsilon)) {
                        // Points are below the face
                        if (dot1 > epsilon) { // Points are above the face
                            hull.AddFaceFromVertices(vIds[i], vIds[k], vIds[j]);
                        } else { // Found a valid face! Add it with correct orientation
                            hull.AddFaceFromVertices(vIds[i], vIds[j], vIds[k]);
                        }
                        foundValidFace = true;  // Set flag to break all loops
                        break;  // Break innermost loop
                    }
                }
            }
        }

        while (hull.openEdgesQueue.size() > 0){

            foundValidFace = false;
            int openEdgeId = hull.openEdgesQueue.front();
            int openEdgeStartVertexID = hull.edges[openEdgeId].vStartId; 
            int openEdgeEndVertexID = hull.edges[openEdgeId].vEndId; 

            std::vector<int> candidateTriangle;
            std::vector<std::vector<int>> candidateTriangleList;
            int bestTriangleIndex, bestTriangleScore;
            
            for(int i = 0; i < 5; i++){
                if(openEdgeStartVertexID != i && openEdgeEndVertexID != i){
                    glm::vec3 A(group[openEdgeEndVertexID].x, group[openEdgeEndVertexID].y, group[openEdgeEndVertexID].z);
                    glm::vec3 B(group[openEdgeStartVertexID].x, group[openEdgeStartVertexID].y, group[openEdgeStartVertexID].z);
                    glm::vec3 C(group[i].x, group[i].y, group[i].z);

                    glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
                    if (glm::length(normal) < epsilon) continue;

                    float dot1 = -1, dot2 = -1;
                    int remainingIdx1 = -1, remainingIdx2 = -1;
                    int currentRem = 0;

                    // Find the two remaining points and test them
                    for (int l = 0; l < 5; l++) {
                        if (l != i && l != openEdgeEndVertexID && l != openEdgeStartVertexID) {
                            glm::vec3 P(group[l].x, group[l].y, group[l].z);
                            if (currentRem == 0) {
                                dot1 = glm::dot(P - A, normal);
                                remainingIdx1 = l;
                            } else {
                                dot2 = glm::dot(P - A, normal);
                                remainingIdx2 = l;
                            }
                            currentRem++;
                        }
                    }

                    if  ((dot1 < epsilon || dot1 == 0) && (dot2 < epsilon || dot2 == 0)){
                        candidateTriangle.push_back(vIds[openEdgeEndVertexID]);
                        candidateTriangle.push_back(vIds[openEdgeStartVertexID]);
                        candidateTriangle.push_back(vIds[i]);

                        candidateTriangleList.push_back(candidateTriangle);
                        candidateTriangle.clear();
                    }
                }
            }

            int vertex1ID, vertex2ID, vertex3ID, triangleScore;

            for (int i = 0; i < candidateTriangleList.size(); i++){
                if (i == 0){
                    bestTriangleIndex = 0;
                    vertex1ID = candidateTriangleList[0][0];
                    vertex2ID = candidateTriangleList[0][1];
                    vertex3ID = candidateTriangleList[0][2];
                    bestTriangleScore = hull.CheckHowManyEdgesTriangleRemovesFromQueue(vertex1ID, vertex2ID, vertex3ID);
                }

                else {
                    vertex1ID = candidateTriangleList[i][0];
                    vertex2ID = candidateTriangleList[i][1];
                    vertex3ID = candidateTriangleList[i][2];
                    triangleScore = hull.CheckHowManyEdgesTriangleRemovesFromQueue(vertex1ID, vertex2ID, vertex3ID);
                    if(triangleScore > bestTriangleScore){
                        bestTriangleIndex = i;
                        bestTriangleScore = triangleScore;
                    }
                }
            }

            vertex1ID = candidateTriangleList[bestTriangleIndex][0];
            vertex2ID = candidateTriangleList[bestTriangleIndex][1];
            vertex3ID = candidateTriangleList[bestTriangleIndex][2];

            hull.AddFaceFromVertices(vIds[vertex1ID], vIds[vertex2ID], vIds[vertex3ID]);
        }
    }

    //hull.DebugPrint();

    return hull;
}

bool Mesh::FaceSeesOtherHull(glm::vec3 faceNormal, glm::vec3 faceVertex, std::vector<glm::vec3> otherHullVertices){
    const float epsilon = 1e-6f;
    float dot;

    for (const auto& vertex : otherHullVertices){
        dot = glm::dot(vertex - faceVertex, faceNormal);
        if(dot > 0){
            return true;
        }
    }

    return false;
}

void Mesh::MergeHull(){
    int mergeStep = 0;
    bool allPartitionsDone = false;
    bool thereAreTwoOrMoreHullsToMerge;

    std::vector<WingedEdgeMesh> mergingHulls;
    std::vector<std::vector<WingedEdgeMesh>> mergingHullsPartitions;

    //while (!allPartitionsDone){
        for (int partition = 0; partition < mergeHullPartitionsColections[mergeStep].size();partition++){
            for (int hull = 0; hull < mergeHullPartitionsColections[mergeStep][partition].size(); hull+=2){
            
                thereAreTwoOrMoreHullsToMerge = (hull+2 <= mergeHullPartitionsColections[mergeStep][partition].size());

                if(thereAreTwoOrMoreHullsToMerge){
                    mergingHulls.push_back(MergeTwoHulls(mergeHullPartitionsColections[mergeStep][partition][hull], mergeHullPartitionsColections[mergeStep][partition][hull+1]));
                }
                else 
                    mergingHulls.push_back(mergeHullPartitionsColections[mergeStep][partition][hull]);
            }

            mergingHullsPartitions.push_back(mergingHulls);
            mergingHulls.clear();
        }

        mergeHullPartitionsColections.push_back(mergingHullsPartitions);
        mergingHullsPartitions.clear();
        mergeStep++;
        allPartitionsDone = true;

        for(int partitionIndex = 0; partitionIndex < mergeHullPartitionsColections[mergeStep].size(); partitionIndex++){
            if(mergeHullPartitionsColections[mergeStep][partitionIndex].size() > 1)
                allPartitionsDone = false;
        }

    //}
}

WingedEdgeMesh Mesh::LinkHulls(WingedEdgeMesh leftHull, WingedEdgeMesh rightHull){
    leftHull.leftHullOpenEdgesQueue = leftHull.openEdgesQueue;

    int leftHullVerticesAmount = leftHull.vertices.size();
    int leftHullEdgesAmount = leftHull.edges.size();
    int leftHullFacesAmount = leftHull.faces.size();
    
    rightHull.AppendDataToLinkHulls(leftHullVerticesAmount, leftHullEdgesAmount, leftHullFacesAmount);

    for (int i = 0; i < rightHull.vertices.size(); i++){
        leftHull.vertices.push_back(rightHull.vertices[i]);
    }
    
    for (int i = 0; i < rightHull.edges.size(); i++){
        leftHull.edges.push_back(rightHull.edges[i]);
    }

    for (int i = 0; i < rightHull.faces.size(); i++){
        leftHull.faces.push_back(rightHull.faces[i]);
    }

    leftHull.rightHullOpenEdgesQueue = rightHull.openEdgesQueue;

    for(int i = 0; i < rightHull.openEdgesQueue.size(); i++){
        leftHull.openEdgesQueue.push_back(rightHull.openEdgesQueue[i]);
    }

    return leftHull;
}

WingedEdgeMesh Mesh::MergeTwoHulls(WingedEdgeMesh leftHull, WingedEdgeMesh rightHull){

    bool faceSeesOtherHull, hullsAreColinear = true;
    glm::vec3 faceVertexPosition;
    glm::vec3 faceNormal;

    int edgeId, vertexID;

    std::vector<glm::vec3> leftHullVerticesPositions = leftHull.ExtractVerticesPositions();
    std::vector<glm::vec3> rightHullVerticesPositions = rightHull.ExtractVerticesPositions();

    //std::cout << "leftHullfaces.size(): " << leftHull.faces.size() << std::endl;

    //leftHull.DebugPrint();


    for(int faceId = 0; faceId < leftHull.faces.size(); faceId++){
        faceNormal = leftHull.faces[faceId].faceNormal;

        edgeId = leftHull.faces[faceId].edgeId;
        vertexID = leftHull.edges[edgeId].vStartId;
        faceVertexPosition = leftHull.vertices[vertexID].position;

        faceSeesOtherHull = FaceSeesOtherHull(faceNormal, faceVertexPosition, rightHullVerticesPositions);

        if(faceSeesOtherHull){
            leftHull.DeleteFace(faceId, rightHullVerticesPositions);
            hullsAreColinear = false;
        }
    }

    for(int faceId = 0; faceId < rightHull.faces.size(); faceId++){
        faceNormal = rightHull.faces[faceId].faceNormal;

        edgeId = rightHull.faces[faceId].edgeId;
        vertexID = rightHull.edges[edgeId].vStartId;
        faceVertexPosition = rightHull.vertices[vertexID].position;

        faceSeesOtherHull = FaceSeesOtherHull(faceNormal, faceVertexPosition, leftHullVerticesPositions);

        if(faceSeesOtherHull){
            rightHull.DeleteFace(faceId, leftHullVerticesPositions);
            hullsAreColinear = false;
        }
    }

    if(hullsAreColinear){
        
    }


    leftHull.FixMeshAfterDeletions();
    
    rightHull.FixMeshAfterDeletions();

    WingedEdgeMesh resultHull = LinkHulls(leftHull, rightHull);


    resultHull.AddFirstMergingEdge();

    //resultHull.SewHulls();

    resultHull.DebugPrint();

    //std::cout << "LEFT HULL OPEN EDGES: " << std::endl;
    //for (int i = 0 ; i < resultHull.leftHullOpenEdgesQueue.size(); i++){
    //    std::cout << resultHull.leftHullOpenEdgesQueue[i] << " " << std::endl;
    //}
    //
    //std::cout << " RIGHT HULL OPEN EDGES:" << std::endl;
    //for (int i = 0 ; i < resultHull.rightHullOpenEdgesQueue.size(); i++){
    //    std::cout << resultHull.rightHullOpenEdgesQueue[i] << " " << std::endl;
    //}
//
    //std::cout << " ALL OPEN EDGES:" << std::endl;
    //for (int i = 0 ; i < resultHull.openEdgesQueue.size(); i++){
    //    std::cout << resultHull.openEdgesQueue[i] << " " << std::endl;
    //}

    return resultHull;
}