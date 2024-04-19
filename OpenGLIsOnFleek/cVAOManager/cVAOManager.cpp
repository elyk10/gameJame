#include "cVAOManager.h"

#include "../OpenGLCommon.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>
#include <fstream>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BoneWeightInfo {
    BoneWeightInfo() {
        m_BoneId[0] = 0;
        m_BoneId[1] = 0;
        m_BoneId[2] = 0;
        m_BoneId[3] = 0;
        m_Weight[0] = 0.f;
        m_Weight[1] = 0.f;
        m_Weight[2] = 0.f;
        m_Weight[3] = 0.f;
    }
    float m_BoneId[4];
    float m_Weight[4];
};

Assimp::Importer m_AssimpImporter; 

//void AssimpToGLM(const aiMatrix4x4& a, glm::mat4& g)
//{
//    g[0][0] = a.a1; g[0][1] = a.b1; g[0][2] = a.c1; g[0][3] = a.d1;
//    g[1][0] = a.a2; g[1][1] = a.b2; g[1][2] = a.c2; g[1][3] = a.d2;
//    g[2][0] = a.a3; g[2][1] = a.b3; g[2][2] = a.c3; g[2][3] = a.d3;
//    g[3][0] = a.a4; g[3][1] = a.b4; g[3][2] = a.c4; g[3][3] = a.d4;
//} 

void cVAOManager::setBasePath(std::string basePathWithoutSlash)
{
    this->m_basePathWithoutSlash = basePathWithoutSlash;
    return;
}

std::string cVAOManager::getBasePath(void)
{
    return this->m_basePathWithoutSlash;
}

bool cVAOManager::LoadModelIntoVAO(std::string fileName,
                      unsigned int shaderProgramID)
{
    sModelDrawInfo tempModelInfo;
    return this->LoadModelIntoVAO(fileName, tempModelInfo, shaderProgramID);
}

bool cVAOManager::LoadAnimationFile(std::string fileName,
    sModelDrawInfo& drawInfo)
{
    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;

    aiScene* scene = (aiScene*)m_AssimpImporter.ReadFile(fileAndPath, aiProcess_GenNormals);

    if (scene == 0 || scene->mNumAnimations < 1)
    {
        return false;
    }
    else
    {
        CharacterAnimation* characterAnimation = new CharacterAnimation();
        aiAnimation* animation = scene->mAnimations[0];

        characterAnimation->Name = animation->mName.C_Str();
        characterAnimation->duration = animation->mDuration;
        characterAnimation->ticksPerSecond = animation->mTicksPerSecond;
        characterAnimation->currentTime = 0.0;

        for (int i = 0; i < animation->mNumChannels; i++)
        {
            aiNodeAnim* assimpNodeAnim = animation->mChannels[i];
            NodeAnim* nodeAnim = new NodeAnim(assimpNodeAnim->mNodeName.C_Str());

            for (int j = 0; j < assimpNodeAnim->mNumPositionKeys; ++j)
            {
                aiVectorKey& p = assimpNodeAnim->mPositionKeys[j];
                nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(glm::vec3(p.mValue.x, p.mValue.y, p.mValue.z), p.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumScalingKeys; ++j)
            {
                aiVectorKey& s = assimpNodeAnim->mScalingKeys[j];
                nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(glm::vec3(s.mValue.x, s.mValue.y, s.mValue.z), s.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumRotationKeys; ++j)
            {
                aiQuatKey& q = assimpNodeAnim->mRotationKeys[j];
                nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(glm::quat(q.mValue.w, q.mValue.x, q.mValue.y, q.mValue.z), q.mTime));
            }

            characterAnimation->Channels.emplace_back(nodeAnim);
        }

        drawInfo.CharacterAnimations.push_back(characterAnimation);
    }

    return true;    
}

bool cVAOManager::LoadModelWithAssimpMultipleAnimations(std::string fileName,
    std::vector<std::string> animationFiles,
    sModelDrawInfo& drawInfo,
    unsigned int shaderProgramID,unsigned int shadowShader, 
    bool bIsDynamicBuffer /*=false*/)
{
    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;
    drawInfo.meshName = fileName;

    aiScene* scene = (aiScene*)m_AssimpImporter.ReadFile(fileAndPath, aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace);//aiProcess_GenSmoothNormals);

    if (scene == 0 || !scene->HasMeshes())
    {
        return false;
    }

    aiMesh* mesh = scene->mMeshes[0];

    if (scene->mNumAnimations > 0)
    {
        
        CharacterAnimation* characterAnimation = new CharacterAnimation();
        aiAnimation* animation = scene->mAnimations[0];

        characterAnimation->Name = animation->mName.C_Str();
        characterAnimation->duration = animation->mDuration;
        characterAnimation->ticksPerSecond = animation->mTicksPerSecond;
        characterAnimation->currentTime = 0.0;

        for (int i = 0; i < animation->mNumChannels; i++)
        {
            aiNodeAnim* assimpNodeAnim = animation->mChannels[i];
            NodeAnim* nodeAnim = new NodeAnim(assimpNodeAnim->mNodeName.C_Str());

            for (int j = 0; j < assimpNodeAnim->mNumPositionKeys; ++j)
            {
                aiVectorKey& p = assimpNodeAnim->mPositionKeys[j];
                nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(glm::vec3(p.mValue.x, p.mValue.y, p.mValue.z), p.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumScalingKeys; ++j)
            {
                aiVectorKey& s = assimpNodeAnim->mScalingKeys[j];
                nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(glm::vec3(s.mValue.x, s.mValue.y, s.mValue.z), s.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumRotationKeys; ++j)
            {
                aiQuatKey& q = assimpNodeAnim->mRotationKeys[j];
                nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(glm::quat(q.mValue.w, q.mValue.x, q.mValue.y, q.mValue.z), q.mTime));
            }

            characterAnimation->Channels.emplace_back(nodeAnim);
        }
        
        drawInfo.CharacterAnimations.push_back(characterAnimation);
    }

    
    
    drawInfo.RootNode = drawInfo.GenerateBoneHierarchy(scene->mRootNode);
    drawInfo.GlobalInverseTransformation = glm::inverse(drawInfo.RootNode->transformation);

    std::vector<BoneWeightInfo> boneWeights;

    if (mesh->HasBones())
    {
        boneWeights.resize(mesh->mNumVertices);
        unsigned int numBones = mesh->mNumBones;

        for (unsigned int boneIdx = 0; boneIdx < numBones; ++boneIdx)
        {
            aiBone* bone = mesh->mBones[boneIdx];

            std::string name(bone->mName.C_Str(), bone->mName.length);
            if (drawInfo.BoneNameToIdMap.find(bone->mName.C_Str()) == drawInfo.BoneNameToIdMap.end())
            {
                drawInfo.BoneNameToIdMap.insert(std::pair<std::string, int>(name, drawInfo.BoneInfoVec.size()));
            }

            // store offset matrices
            BoneInfo info;
            drawInfo.AssimpToGLM(bone->mOffsetMatrix, info.BoneOffset);
            drawInfo.BoneInfoVec.emplace_back(info);


            for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
            {
                aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
                // BoneId		:	boneIdx
                // Vertexid		:	vertexWeight.mVertexId
                // Weight		:	vertexWeight.mWeight



                BoneWeightInfo& boneInfo = boneWeights[vertexWeight.mVertexId];
                for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
                {
                    if (boneInfo.m_Weight[infoIdx] == 0.0f)
                    {
                        boneInfo.m_BoneId[infoIdx] = boneIdx;
                        boneInfo.m_Weight[infoIdx] = vertexWeight.mWeight;
                        break;
                    }
                }
            }
        }
    }
    
    
    

    unsigned int numVerticesInVertArray = mesh->mNumVertices;
    unsigned int numIndicesInIndexArray = mesh->mNumFaces * 3;

    drawInfo.numberOfVertices = numVerticesInVertArray;
    drawInfo.numberOfTriangles = mesh->mNumFaces;
    drawInfo.numberOfIndices = numIndicesInIndexArray;
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    std::vector<unsigned int>triangles;

    sVertex* pTempVertArray = new sVertex[numIndicesInIndexArray * 2];

    GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray * 2];

    int count = 0;
    int index = 0;

    unsigned int vertArrayIndex = 0;
    for (unsigned int faceIdx = 0; faceIdx != mesh->mNumFaces; faceIdx++)
    {
        aiFace face = mesh->mFaces[faceIdx];

        for (int idx = 0; idx != 3; idx++)
        {
            unsigned int index = face.mIndices[idx];

            triangles.push_back(index);


            aiVector3D position = mesh->mVertices[index];
            pTempVertArray[vertArrayIndex].x = position.x;
            pTempVertArray[vertArrayIndex].y = position.y;
            pTempVertArray[vertArrayIndex].z = position.z;
            pTempVertArray[vertArrayIndex].w = 1.0f;

            if (mesh->HasTextureCoords(0))
            {
                aiVector3D textureCoord = mesh->mTextureCoords[0][index];
                pTempVertArray[vertArrayIndex].u = textureCoord.x;
                pTempVertArray[vertArrayIndex].v = textureCoord.y;
                pTempVertArray[vertArrayIndex].u2 = textureCoord.z;
                pTempVertArray[vertArrayIndex].v2 = 0.0f;
            }
            else
            {
                pTempVertArray[vertArrayIndex].u = 0.0f;
                pTempVertArray[vertArrayIndex].v = 0.0f;
                pTempVertArray[vertArrayIndex].u2 = 0.0f;
                pTempVertArray[vertArrayIndex].v2 = 0.0f;
            }

            aiVector3D normal = mesh->mNormals[index];
            pTempVertArray[vertArrayIndex].nx = normal.x;
            pTempVertArray[vertArrayIndex].ny = normal.y;
            pTempVertArray[vertArrayIndex].nz = normal.z;
            pTempVertArray[vertArrayIndex].nw = 0.0f;

            if (mesh->HasBones())
            {
                BoneWeightInfo& boneInfo = boneWeights[index];

                pTempVertArray[vertArrayIndex].bone1 = boneInfo.m_BoneId[0];
                pTempVertArray[vertArrayIndex].bone2 = boneInfo.m_BoneId[1];
                pTempVertArray[vertArrayIndex].bone3 = boneInfo.m_BoneId[2];
                pTempVertArray[vertArrayIndex].bone4 = boneInfo.m_BoneId[3];

                pTempVertArray[vertArrayIndex].bWeight1 = boneInfo.m_Weight[0];
                pTempVertArray[vertArrayIndex].bWeight2 = boneInfo.m_Weight[1];
                pTempVertArray[vertArrayIndex].bWeight3 = boneInfo.m_Weight[2];
                pTempVertArray[vertArrayIndex].bWeight4 = boneInfo.m_Weight[3];

                // float weight = boneInfo.m_Weight[0] + boneInfo.m_Weight[1] + boneInfo.m_Weight[2] + boneInfo.m_Weight[3];

            }


            if (mesh->HasVertexColors(0))
            {
                aiColor4D colour = mesh->mColors[0][index];
                pTempVertArray[vertArrayIndex].r = colour.r;
                pTempVertArray[vertArrayIndex].g = colour.g;
                pTempVertArray[vertArrayIndex].b = colour.b;
                pTempVertArray[vertArrayIndex].a = colour.a;
            }
            else
            {
                pTempVertArray[vertArrayIndex].r = 1.0f;
                pTempVertArray[vertArrayIndex].g = 1.0f;
                pTempVertArray[vertArrayIndex].b = 1.0f;
                pTempVertArray[vertArrayIndex].a = 1.0f;
            }

            if (mesh->HasTangentsAndBitangents())
            {
                aiVector3D tangent = mesh->mTangents[index];
                aiVector3D bitangent = mesh->mBitangents[index];

                pTempVertArray[vertArrayIndex].tx = tangent.x;
                pTempVertArray[vertArrayIndex].ty = tangent.y;
                pTempVertArray[vertArrayIndex].tz = tangent.z;
                pTempVertArray[vertArrayIndex].tw = 0.0f;
                
                pTempVertArray[vertArrayIndex].bx = bitangent.x;
                pTempVertArray[vertArrayIndex].by = bitangent.y;
                pTempVertArray[vertArrayIndex].bz = bitangent.z;
                pTempVertArray[vertArrayIndex].bw = 0.0f;
            }
            else
            {
                pTempVertArray[vertArrayIndex].tx = 0.0f;
                pTempVertArray[vertArrayIndex].ty = 0.0f;
                pTempVertArray[vertArrayIndex].tz = 0.0f;
                pTempVertArray[vertArrayIndex].tw = 0.0f;

                pTempVertArray[vertArrayIndex].bx = 0.0f;
                pTempVertArray[vertArrayIndex].by = 0.0f;
                pTempVertArray[vertArrayIndex].bz = 0.0f;
                pTempVertArray[vertArrayIndex].bw = 0.0f;
            }


            pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;

            vertArrayIndex++;
        }
    }

    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];

    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        drawInfo.pVertices[vertIndex].x = pTempVertArray[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTempVertArray[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTempVertArray[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = pTempVertArray[vertIndex].w;

        drawInfo.pVertices[vertIndex].nx = pTempVertArray[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTempVertArray[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTempVertArray[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = pTempVertArray[vertIndex].nw;

        drawInfo.pVertices[vertIndex].u = pTempVertArray[vertIndex].u;
        drawInfo.pVertices[vertIndex].v = pTempVertArray[vertIndex].v;
        drawInfo.pVertices[vertIndex].u2 = pTempVertArray[vertIndex].u2;
        drawInfo.pVertices[vertIndex].v2 = pTempVertArray[vertIndex].v2;

        drawInfo.pVertices[vertIndex].tx = pTempVertArray[vertIndex].tx;
        drawInfo.pVertices[vertIndex].ty = pTempVertArray[vertIndex].ty;
        drawInfo.pVertices[vertIndex].tz = pTempVertArray[vertIndex].tz;
        drawInfo.pVertices[vertIndex].tw = pTempVertArray[vertIndex].tw;
        
        drawInfo.pVertices[vertIndex].bx = pTempVertArray[vertIndex].bx;
        drawInfo.pVertices[vertIndex].by = pTempVertArray[vertIndex].by;
        drawInfo.pVertices[vertIndex].bz = pTempVertArray[vertIndex].bz;
        drawInfo.pVertices[vertIndex].bw = pTempVertArray[vertIndex].bw;

        /*drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTempVertArray[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTempVertArray[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTempVertArray[vertIndex].a;
        */

        drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].bone1 / 52.0f;
        drawInfo.pVertices[vertIndex].g = 0.0f;//pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].b = 0.0f;//pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].a = 1.0f;//pTempVertArray[vertIndex].bone1;


        drawInfo.pVertices[vertIndex].bone1 = pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].bone2 = pTempVertArray[vertIndex].bone2;
        drawInfo.pVertices[vertIndex].bone3 = pTempVertArray[vertIndex].bone3;
        drawInfo.pVertices[vertIndex].bone4 = pTempVertArray[vertIndex].bone4;

        drawInfo.pVertices[vertIndex].bWeight1 = pTempVertArray[vertIndex].bWeight1;
        drawInfo.pVertices[vertIndex].bWeight2 = pTempVertArray[vertIndex].bWeight2;
        drawInfo.pVertices[vertIndex].bWeight3 = pTempVertArray[vertIndex].bWeight3;
        drawInfo.pVertices[vertIndex].bWeight4 = pTempVertArray[vertIndex].bWeight4;
    }

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; ++triIndex)
    {
        drawInfo.pIndices[elementIndex + 0] = triangles[elementIndex + 0];
        drawInfo.pIndices[elementIndex + 1] = triangles[elementIndex + 1];
        drawInfo.pIndices[elementIndex + 2] = triangles[elementIndex + 2];

               
        if (mesh->HasNormals() && mesh->HasTextureCoords(0) && !mesh->HasTangentsAndBitangents())
        {
            sVertex vertexOne = drawInfo.pVertices[triangles[elementIndex]];
            sVertex vertexTwo = drawInfo.pVertices[triangles[elementIndex + 1]];
            sVertex vertexThree = drawInfo.pVertices[triangles[elementIndex + 2]];

            glm::vec3 edgeOne = glm::vec3(vertexTwo.x, vertexTwo.y, vertexTwo.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);
            glm::vec3 edgeTwo = glm::vec3(vertexThree.x, vertexThree.y, vertexThree.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);

            glm::vec2 deltaUVOne = glm::vec2(vertexTwo.u, vertexTwo.v) - glm::vec2(vertexOne.u, vertexOne.v);
            glm::vec2 deltaUVTwo = glm::vec2(vertexThree.u, vertexThree.v) - glm::vec2(vertexOne.u, vertexOne.v);

            float fractional = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVTwo.x * deltaUVOne.y);

            glm::vec3 tangent;
            glm::vec3 bitangent;

            tangent.x = fractional * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
            tangent.y = fractional * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
            tangent.z = fractional * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);
            
            bitangent.x = fractional * (-deltaUVTwo.x * edgeOne.x - deltaUVOne.x * edgeTwo.x);
            bitangent.y = fractional * (-deltaUVTwo.x * edgeOne.y - deltaUVOne.x * edgeTwo.y);
            bitangent.z = fractional * (-deltaUVTwo.x * edgeOne.z - deltaUVOne.x * edgeTwo.z);

            glm::normalize(tangent);
            glm::normalize(bitangent);

            
            for (int i = 0; i < 3; i++)
            {
                drawInfo.pVertices[triangles[elementIndex + i]].tx = tangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].ty = tangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].tz = tangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].tw = 0.0f;

                drawInfo.pVertices[triangles[elementIndex + i]].bx = bitangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].by = bitangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].bz = bitangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].bw = 0.0f;
            }

        }

        elementIndex += 3;
    }



    // 
    // Model is loaded and the vertices and indices are in the drawInfo struct
    // 

    // Create a VAO (Vertex Array Object), which will 
    //	keep track of all the 'state' needed to draw 
    //	from this buffer...
    glUseProgram(shaderProgramID);
    // Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(drawInfo.VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(drawInfo.VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 


    // NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &(drawInfo.VertexBufferID));

    //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
    // sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
        (GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
        (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));


    // Copy the index buffer into the video card, too
    // Create an index buffer.
    glGenBuffers(1, &(drawInfo.IndexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
        sizeof(unsigned int) * drawInfo.numberOfIndices,
        (GLvoid*)drawInfo.pIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.
   	// program 

    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location = glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
    GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
    GLint vBitangent_location = glGetAttribLocation(shaderProgramID, "vBitangent");
    GLint vBoneIndex_location = glGetAttribLocation(shaderProgramID, "vBoneIndex");
    GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));
    
    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, nx));

    glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
    glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, u));

    glEnableVertexAttribArray(vTangent_location);	    // vTangent;
    glVertexAttribPointer(vTangent_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, tx));
    
    glEnableVertexAttribArray(vBitangent_location);	    // vBoneIndex;
    glVertexAttribPointer(vBitangent_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bx));
    
    glEnableVertexAttribArray(vBoneIndex_location);	    // vBoneIndex;
    glVertexAttribPointer(vBoneIndex_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bone1));

    glEnableVertexAttribArray(vBoneWeight_location);	    // vBoneWeight;
    glVertexAttribPointer(vBoneWeight_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bWeight1));

    glUseProgram(shadowShader);

    GLint vpos_location_shadow = glGetAttribLocation(shadowShader, "vPos");	// program
    GLint vcol_location_shadow = glGetAttribLocation(shadowShader, "vCol");	// program;
    GLint vNormal_location_shadow = glGetAttribLocation(shadowShader, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location_shadow = glGetAttribLocation(shadowShader, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
    GLint vTangent_location_shadow = glGetAttribLocation(shadowShader, "vTangent");
    GLint vBitangent_location_shadow = glGetAttribLocation(shadowShader, "vBitangent");
    GLint vBoneIndex_location_shadow = glGetAttribLocation(shadowShader, "vBoneIndex");
    GLint vBoneWeight_location_shadow = glGetAttribLocation(shadowShader, "vBoneWeight"); // Now that all the parts are set up, set the VAO to zero
    
     glEnableVertexAttribArray(vpos_location_shadow);	    // vPos
    glVertexAttribPointer(vpos_location_shadow, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));

    glEnableVertexAttribArray(vcol_location_shadow);	    // vCol
    glVertexAttribPointer(vcol_location_shadow, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glEnableVertexAttribArray(vNormal_location_shadow);	// vNormal
    glVertexAttribPointer(vNormal_location_shadow, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, nx));

    glEnableVertexAttribArray(vTextureCoords_location_shadow);	    // vTextureCoords;
    glVertexAttribPointer(vTextureCoords_location_shadow, 2,		// in vec2 vTextureCoords;
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, u));

    glEnableVertexAttribArray(vTangent_location_shadow);	    // vTangent;
    glVertexAttribPointer(vTangent_location_shadow, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, tx));
    
    glEnableVertexAttribArray(vBitangent_location_shadow);	    // vBoneIndex;
    glVertexAttribPointer(vBitangent_location_shadow, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bx));
    
    glEnableVertexAttribArray(vBoneIndex_location_shadow);	    // vBoneIndex;
    glVertexAttribPointer(vBoneIndex_location_shadow, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bone1));

    glEnableVertexAttribArray(vBoneWeight_location_shadow);	    // vBoneWeight;
    glVertexAttribPointer(vBoneWeight_location_shadow, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bWeight1));
    
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDisableVertexAttribArray(vNormal_location);
    glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
    glDisableVertexAttribArray(vTangent_location);        // <-- New
    glDisableVertexAttribArray(vBitangent_location);        // <-- New
    glDisableVertexAttribArray(vBoneIndex_location);        // <-- New
    glDisableVertexAttribArray(vBoneWeight_location);        // <-- New

    
   
    // Ask OpenGL for a new buffer ID...
   // glGenVertexArrays(1, &(drawInfo.VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    //glBindVertexArray(drawInfo.VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 


    // NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
   // glGenBuffers(1, &(drawInfo.VertexBufferID));

    //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
   // glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
    // sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
   // glBufferData(GL_ARRAY_BUFFER,
   //     sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
   //     (GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
   //     (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));


    // Copy the index buffer into the video card, too
    // Create an index buffer.
   // glGenBuffers(1, &(drawInfo.IndexBufferID));

   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

   // glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
   //     sizeof(unsigned int) * drawInfo.numberOfIndices,
   //     (GLvoid*)drawInfo.pIndices,
   //     GL_STATIC_DRAW);

    // Set the vertex attributes.

   

    // Set the vertex attributes for this shader
   

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location_shadow);
    glDisableVertexAttribArray(vcol_location_shadow);
    glDisableVertexAttribArray(vNormal_location_shadow);
    glDisableVertexAttribArray(vTextureCoords_location_shadow);        // <-- New
    glDisableVertexAttribArray(vTangent_location_shadow);        // <-- New
    glDisableVertexAttribArray(vBitangent_location_shadow);        // <-- New
    glDisableVertexAttribArray(vBoneIndex_location_shadow);        // <-- New
    glDisableVertexAttribArray(vBoneWeight_location_shadow);        // <-- New

    
    
    if (animationFiles.size() > 0)
    {
        for (int i = 0; i < animationFiles.size(); i++)
        {
            LoadAnimationFile(animationFiles[i], drawInfo);
        }
    }
    // Store the draw information into the map
    this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;
    
    //glUseProgram(shaderProgramID);

    return true;
}

bool cVAOManager::LoadModelWithAssimp(std::string fileName,
    sModelDrawInfo& drawInfo,
    unsigned int shaderProgramID,
    bool bIsDynamicBuffer /*=false*/)
{
    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;
    drawInfo.meshName = fileName;

    aiScene* scene = (aiScene*)m_AssimpImporter.ReadFile(fileAndPath, aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace);//aiProcess_GenSmoothNormals);

    if (scene == 0 || !scene->HasMeshes())
    {
        return false;
    }

    aiMesh* mesh = scene->mMeshes[0];

    if (scene->mNumAnimations > 0)
    {
        
        CharacterAnimation* characterAnimation = new CharacterAnimation();
        aiAnimation* animation = scene->mAnimations[0];

        characterAnimation->Name = animation->mName.C_Str();
        characterAnimation->duration = animation->mDuration;
        characterAnimation->ticksPerSecond = animation->mTicksPerSecond;
        characterAnimation->currentTime = 0.0;

        for (int i = 0; i < animation->mNumChannels; i++)
        {
            aiNodeAnim* assimpNodeAnim = animation->mChannels[i];
            NodeAnim* nodeAnim = new NodeAnim(assimpNodeAnim->mNodeName.C_Str());

            for (int j = 0; j < assimpNodeAnim->mNumPositionKeys; ++j)
            {
                aiVectorKey& p = assimpNodeAnim->mPositionKeys[j];
                nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(glm::vec3(p.mValue.x, p.mValue.y, p.mValue.z), p.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumScalingKeys; ++j)
            {
                aiVectorKey& s = assimpNodeAnim->mScalingKeys[j];
                nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(glm::vec3(s.mValue.x, s.mValue.y, s.mValue.z), s.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumRotationKeys; ++j)
            {
                aiQuatKey& q = assimpNodeAnim->mRotationKeys[j];
                nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(glm::quat(q.mValue.w, q.mValue.x, q.mValue.y, q.mValue.z), q.mTime));
            }

            characterAnimation->Channels.emplace_back(nodeAnim);
        }
        
        drawInfo.CharacterAnimations.push_back(characterAnimation);
    }

    
    
    drawInfo.RootNode = drawInfo.GenerateBoneHierarchy(scene->mRootNode);
    drawInfo.GlobalInverseTransformation = glm::inverse(drawInfo.RootNode->transformation);

    std::vector<BoneWeightInfo> boneWeights;

    if (mesh->HasBones())
    {
        boneWeights.resize(mesh->mNumVertices);
        unsigned int numBones = mesh->mNumBones;

        for (unsigned int boneIdx = 0; boneIdx < numBones; ++boneIdx)
        {
            aiBone* bone = mesh->mBones[boneIdx];

            std::string name(bone->mName.C_Str(), bone->mName.length);
            if (drawInfo.BoneNameToIdMap.find(bone->mName.C_Str()) == drawInfo.BoneNameToIdMap.end())
            {
                drawInfo.BoneNameToIdMap.insert(std::pair<std::string, int>(name, drawInfo.BoneInfoVec.size()));
            }

            // store offset matrices
            BoneInfo info;
            drawInfo.AssimpToGLM(bone->mOffsetMatrix, info.BoneOffset);
            drawInfo.BoneInfoVec.emplace_back(info);


            for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
            {
                aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
                // BoneId		:	boneIdx
                // Vertexid		:	vertexWeight.mVertexId
                // Weight		:	vertexWeight.mWeight



                BoneWeightInfo& boneInfo = boneWeights[vertexWeight.mVertexId];
                for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
                {
                    if (boneInfo.m_Weight[infoIdx] == 0.0f)
                    {
                        boneInfo.m_BoneId[infoIdx] = boneIdx;
                        boneInfo.m_Weight[infoIdx] = vertexWeight.mWeight;
                        break;
                    }
                }
            }
        }
    }
    
    
    

    unsigned int numVerticesInVertArray = mesh->mNumVertices;
    unsigned int numIndicesInIndexArray = mesh->mNumFaces * 3;

    drawInfo.numberOfVertices = numVerticesInVertArray;
    drawInfo.numberOfTriangles = mesh->mNumFaces;
    drawInfo.numberOfIndices = numIndicesInIndexArray;
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    std::vector<unsigned int>triangles;

    sVertex* pTempVertArray = new sVertex[numIndicesInIndexArray * 2];

    GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray * 2];

    int count = 0;
    int index = 0;

    unsigned int vertArrayIndex = 0;
    for (unsigned int faceIdx = 0; faceIdx != mesh->mNumFaces; faceIdx++)
    {
        aiFace face = mesh->mFaces[faceIdx];

        for (int idx = 0; idx != 3; idx++)
        {
            unsigned int index = face.mIndices[idx];

            triangles.push_back(index);


            aiVector3D position = mesh->mVertices[index];
            pTempVertArray[vertArrayIndex].x = position.x;
            pTempVertArray[vertArrayIndex].y = position.y;
            pTempVertArray[vertArrayIndex].z = position.z;
            pTempVertArray[vertArrayIndex].w = 1.0f;

            if (mesh->HasTextureCoords(0))
            {
                aiVector3D textureCoord = mesh->mTextureCoords[0][index];
                pTempVertArray[vertArrayIndex].u = textureCoord.x;
                pTempVertArray[vertArrayIndex].v = textureCoord.y;
                pTempVertArray[vertArrayIndex].u2 = textureCoord.z;
                pTempVertArray[vertArrayIndex].v2 = 0.0f;
            }
            else
            {
                pTempVertArray[vertArrayIndex].u = 0.0f;
                pTempVertArray[vertArrayIndex].v = 0.0f;
                pTempVertArray[vertArrayIndex].u2 = 0.0f;
                pTempVertArray[vertArrayIndex].v2 = 0.0f;
            }

            aiVector3D normal = mesh->mNormals[index];
            pTempVertArray[vertArrayIndex].nx = normal.x;
            pTempVertArray[vertArrayIndex].ny = normal.y;
            pTempVertArray[vertArrayIndex].nz = normal.z;
            pTempVertArray[vertArrayIndex].nw = 0.0f;

            if (mesh->HasBones())
            {
                BoneWeightInfo& boneInfo = boneWeights[index];

                pTempVertArray[vertArrayIndex].bone1 = boneInfo.m_BoneId[0];
                pTempVertArray[vertArrayIndex].bone2 = boneInfo.m_BoneId[1];
                pTempVertArray[vertArrayIndex].bone3 = boneInfo.m_BoneId[2];
                pTempVertArray[vertArrayIndex].bone4 = boneInfo.m_BoneId[3];

                pTempVertArray[vertArrayIndex].bWeight1 = boneInfo.m_Weight[0];
                pTempVertArray[vertArrayIndex].bWeight2 = boneInfo.m_Weight[1];
                pTempVertArray[vertArrayIndex].bWeight3 = boneInfo.m_Weight[2];
                pTempVertArray[vertArrayIndex].bWeight4 = boneInfo.m_Weight[3];

                // float weight = boneInfo.m_Weight[0] + boneInfo.m_Weight[1] + boneInfo.m_Weight[2] + boneInfo.m_Weight[3];

            }


            if (mesh->HasVertexColors(0))
            {
                aiColor4D colour = mesh->mColors[0][index];
                pTempVertArray[vertArrayIndex].r = colour.r;
                pTempVertArray[vertArrayIndex].g = colour.g;
                pTempVertArray[vertArrayIndex].b = colour.b;
                pTempVertArray[vertArrayIndex].a = colour.a;
            }
            else
            {
                pTempVertArray[vertArrayIndex].r = 1.0f;
                pTempVertArray[vertArrayIndex].g = 1.0f;
                pTempVertArray[vertArrayIndex].b = 1.0f;
                pTempVertArray[vertArrayIndex].a = 1.0f;
            }

            if (mesh->HasTangentsAndBitangents())
            {
                aiVector3D tangent = mesh->mTangents[index];
                aiVector3D bitangent = mesh->mBitangents[index];

                pTempVertArray[vertArrayIndex].tx = tangent.x;
                pTempVertArray[vertArrayIndex].ty = tangent.y;
                pTempVertArray[vertArrayIndex].tz = tangent.z;
                pTempVertArray[vertArrayIndex].tw = 0.0f;
                
                pTempVertArray[vertArrayIndex].bx = bitangent.x;
                pTempVertArray[vertArrayIndex].by = bitangent.y;
                pTempVertArray[vertArrayIndex].bz = bitangent.z;
                pTempVertArray[vertArrayIndex].bw = 0.0f;
            }
            else
            {
                pTempVertArray[vertArrayIndex].tx = 0.0f;
                pTempVertArray[vertArrayIndex].ty = 0.0f;
                pTempVertArray[vertArrayIndex].tz = 0.0f;
                pTempVertArray[vertArrayIndex].tw = 0.0f;

                pTempVertArray[vertArrayIndex].bx = 0.0f;
                pTempVertArray[vertArrayIndex].by = 0.0f;
                pTempVertArray[vertArrayIndex].bz = 0.0f;
                pTempVertArray[vertArrayIndex].bw = 0.0f;
            }


            pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;

            vertArrayIndex++;
        }
    }

    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];

    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        drawInfo.pVertices[vertIndex].x = pTempVertArray[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTempVertArray[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTempVertArray[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = pTempVertArray[vertIndex].w;

        drawInfo.pVertices[vertIndex].nx = pTempVertArray[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTempVertArray[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTempVertArray[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = pTempVertArray[vertIndex].nw;

        drawInfo.pVertices[vertIndex].u = pTempVertArray[vertIndex].u;
        drawInfo.pVertices[vertIndex].v = pTempVertArray[vertIndex].v;
        drawInfo.pVertices[vertIndex].u2 = pTempVertArray[vertIndex].u2;
        drawInfo.pVertices[vertIndex].v2 = pTempVertArray[vertIndex].v2;

        drawInfo.pVertices[vertIndex].tx = pTempVertArray[vertIndex].tx;
        drawInfo.pVertices[vertIndex].ty = pTempVertArray[vertIndex].ty;
        drawInfo.pVertices[vertIndex].tz = pTempVertArray[vertIndex].tz;
        drawInfo.pVertices[vertIndex].tw = pTempVertArray[vertIndex].tw;
        
        drawInfo.pVertices[vertIndex].bx = pTempVertArray[vertIndex].bx;
        drawInfo.pVertices[vertIndex].by = pTempVertArray[vertIndex].by;
        drawInfo.pVertices[vertIndex].bz = pTempVertArray[vertIndex].bz;
        drawInfo.pVertices[vertIndex].bw = pTempVertArray[vertIndex].bw;

        /*drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTempVertArray[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTempVertArray[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTempVertArray[vertIndex].a;
        */

        drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].bone1 / 52.0f;
        drawInfo.pVertices[vertIndex].g = 0.0f;//pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].b = 0.0f;//pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].a = 1.0f;//pTempVertArray[vertIndex].bone1;


        drawInfo.pVertices[vertIndex].bone1 = pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].bone2 = pTempVertArray[vertIndex].bone2;
        drawInfo.pVertices[vertIndex].bone3 = pTempVertArray[vertIndex].bone3;
        drawInfo.pVertices[vertIndex].bone4 = pTempVertArray[vertIndex].bone4;

        drawInfo.pVertices[vertIndex].bWeight1 = pTempVertArray[vertIndex].bWeight1;
        drawInfo.pVertices[vertIndex].bWeight2 = pTempVertArray[vertIndex].bWeight2;
        drawInfo.pVertices[vertIndex].bWeight3 = pTempVertArray[vertIndex].bWeight3;
        drawInfo.pVertices[vertIndex].bWeight4 = pTempVertArray[vertIndex].bWeight4;
    }

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; ++triIndex)
    {
        drawInfo.pIndices[elementIndex + 0] = triangles[elementIndex + 0];
        drawInfo.pIndices[elementIndex + 1] = triangles[elementIndex + 1];
        drawInfo.pIndices[elementIndex + 2] = triangles[elementIndex + 2];

               
        if (mesh->HasNormals() && mesh->HasTextureCoords(0) && !mesh->HasTangentsAndBitangents())
        {
            sVertex vertexOne = drawInfo.pVertices[triangles[elementIndex]];
            sVertex vertexTwo = drawInfo.pVertices[triangles[elementIndex + 1]];
            sVertex vertexThree = drawInfo.pVertices[triangles[elementIndex + 2]];

            glm::vec3 edgeOne = glm::vec3(vertexTwo.x, vertexTwo.y, vertexTwo.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);
            glm::vec3 edgeTwo = glm::vec3(vertexThree.x, vertexThree.y, vertexThree.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);

            glm::vec2 deltaUVOne = glm::vec2(vertexTwo.u, vertexTwo.v) - glm::vec2(vertexOne.u, vertexOne.v);
            glm::vec2 deltaUVTwo = glm::vec2(vertexThree.u, vertexThree.v) - glm::vec2(vertexOne.u, vertexOne.v);

            float fractional = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVTwo.x * deltaUVOne.y);

            glm::vec3 tangent;
            glm::vec3 bitangent;

            tangent.x = fractional * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
            tangent.y = fractional * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
            tangent.z = fractional * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);
            
            bitangent.x = fractional * (-deltaUVTwo.x * edgeOne.x + deltaUVOne.x * edgeTwo.x);
            bitangent.y = fractional * (-deltaUVTwo.x * edgeOne.y + deltaUVOne.x * edgeTwo.y);
            bitangent.z = fractional * (-deltaUVTwo.x * edgeOne.z + deltaUVOne.x * edgeTwo.z);

            tangent = glm::normalize(tangent);
            bitangent = glm::normalize(bitangent);

            
            for (int i = 0; i < 3; i++)
            {
                drawInfo.pVertices[triangles[elementIndex + i]].tx = tangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].ty = tangent.y;
                drawInfo.pVertices[triangles[elementIndex + i]].tz = tangent.z;
                drawInfo.pVertices[triangles[elementIndex + i]].tw = 1.0f;

                drawInfo.pVertices[triangles[elementIndex + i]].bx = bitangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].by = bitangent.y;
                drawInfo.pVertices[triangles[elementIndex + i]].bz = bitangent.z;
                drawInfo.pVertices[triangles[elementIndex + i]].bw = 1.0f;
            }

        }

        elementIndex += 3;
    }



    // 
    // Model is loaded and the vertices and indices are in the drawInfo struct
    // 

    // Create a VAO (Vertex Array Object), which will 
    //	keep track of all the 'state' needed to draw 
    //	from this buffer...

    // Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(drawInfo.VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(drawInfo.VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 


    // NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &(drawInfo.VertexBufferID));

    //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
    // sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
        (GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
        (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));


    // Copy the index buffer into the video card, too
    // Create an index buffer.
    glGenBuffers(1, &(drawInfo.IndexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
        sizeof(unsigned int) * drawInfo.numberOfIndices,
        (GLvoid*)drawInfo.pIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.

    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location = glad_glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
    GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
    GLint vBitangent_location = glGetAttribLocation(shaderProgramID, "vBitangent");
    GLint vBoneIndex_location = glGetAttribLocation(shaderProgramID, "vBoneIndex");
    GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));

    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, nx));

    glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
    glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, u));

    glEnableVertexAttribArray(vTangent_location);	    // vTangent;
    glVertexAttribPointer(vTangent_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, tx));
    
    glEnableVertexAttribArray(vBitangent_location);	    // vBoneIndex;
    glVertexAttribPointer(vBitangent_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bx));
    
    glEnableVertexAttribArray(vBoneIndex_location);	    // vBoneIndex;
    glVertexAttribPointer(vBoneIndex_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bone1));

    glEnableVertexAttribArray(vBoneWeight_location);	    // vBoneWeight;
    glVertexAttribPointer(vBoneWeight_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bWeight1));

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDisableVertexAttribArray(vNormal_location);
    glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
    glDisableVertexAttribArray(vTangent_location);        // <-- New
    glDisableVertexAttribArray(vBitangent_location);        // <-- New
    glDisableVertexAttribArray(vBoneIndex_location);        // <-- New
    glDisableVertexAttribArray(vBoneWeight_location);        // <-- New

    
    
    // Store the draw information into the map
    this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;
    

    return true;
}

bool cVAOManager::LoadModelWithAssimpShadow(std::string fileName,
    sModelDrawInfo& drawInfo,
    unsigned int shaderProgramID, unsigned int shadowShader, 
    bool bIsDynamicBuffer /*=false*/)
{
    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;
    drawInfo.meshName = fileName;

    aiScene* scene = (aiScene*)m_AssimpImporter.ReadFile(fileAndPath, aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace);//aiProcess_GenSmoothNormals);

    if (scene == 0 || !scene->HasMeshes())
    {
        return false;
    }

    aiMesh* mesh = scene->mMeshes[0];

    if (scene->mNumAnimations > 0)
    {
        
        CharacterAnimation* characterAnimation = new CharacterAnimation();
        aiAnimation* animation = scene->mAnimations[0];

        characterAnimation->Name = animation->mName.C_Str();
        characterAnimation->duration = animation->mDuration;
        characterAnimation->ticksPerSecond = animation->mTicksPerSecond;
        characterAnimation->currentTime = 0.0;

        for (int i = 0; i < animation->mNumChannels; i++)
        {
            aiNodeAnim* assimpNodeAnim = animation->mChannels[i];
            NodeAnim* nodeAnim = new NodeAnim(assimpNodeAnim->mNodeName.C_Str());

            for (int j = 0; j < assimpNodeAnim->mNumPositionKeys; ++j)
            {
                aiVectorKey& p = assimpNodeAnim->mPositionKeys[j];
                nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(glm::vec3(p.mValue.x, p.mValue.y, p.mValue.z), p.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumScalingKeys; ++j)
            {
                aiVectorKey& s = assimpNodeAnim->mScalingKeys[j];
                nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(glm::vec3(s.mValue.x, s.mValue.y, s.mValue.z), s.mTime));
            }
            for (int j = 0; j < assimpNodeAnim->mNumRotationKeys; ++j)
            {
                aiQuatKey& q = assimpNodeAnim->mRotationKeys[j];
                nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(glm::quat(q.mValue.w, q.mValue.x, q.mValue.y, q.mValue.z), q.mTime));
            }

            characterAnimation->Channels.emplace_back(nodeAnim);
        }
        
        drawInfo.CharacterAnimations.push_back(characterAnimation);
    }

    
    
    drawInfo.RootNode = drawInfo.GenerateBoneHierarchy(scene->mRootNode);
    drawInfo.GlobalInverseTransformation = glm::inverse(drawInfo.RootNode->transformation);

    std::vector<BoneWeightInfo> boneWeights;

    if (mesh->HasBones())
    {
        boneWeights.resize(mesh->mNumVertices);
        unsigned int numBones = mesh->mNumBones;

        for (unsigned int boneIdx = 0; boneIdx < numBones; ++boneIdx)
        {
            aiBone* bone = mesh->mBones[boneIdx];

            std::string name(bone->mName.C_Str(), bone->mName.length);
            if (drawInfo.BoneNameToIdMap.find(bone->mName.C_Str()) == drawInfo.BoneNameToIdMap.end())
            {
                drawInfo.BoneNameToIdMap.insert(std::pair<std::string, int>(name, drawInfo.BoneInfoVec.size()));
            }

            // store offset matrices
            BoneInfo info;
            drawInfo.AssimpToGLM(bone->mOffsetMatrix, info.BoneOffset);
            drawInfo.BoneInfoVec.emplace_back(info);


            for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
            {
                aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
                // BoneId		:	boneIdx
                // Vertexid		:	vertexWeight.mVertexId
                // Weight		:	vertexWeight.mWeight



                BoneWeightInfo& boneInfo = boneWeights[vertexWeight.mVertexId];
                for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
                {
                    if (boneInfo.m_Weight[infoIdx] == 0.0f)
                    {
                        boneInfo.m_BoneId[infoIdx] = boneIdx;
                        boneInfo.m_Weight[infoIdx] = vertexWeight.mWeight;
                        break;
                    }
                }
            }
        }
    }
    
    
    

    unsigned int numVerticesInVertArray = mesh->mNumVertices;
    unsigned int numIndicesInIndexArray = mesh->mNumFaces * 3;

    drawInfo.numberOfVertices = numVerticesInVertArray;
    drawInfo.numberOfTriangles = mesh->mNumFaces;
    drawInfo.numberOfIndices = numIndicesInIndexArray;
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    std::vector<unsigned int>triangles;

    sVertex* pTempVertArray = new sVertex[numIndicesInIndexArray * 2];

    GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray * 2];

    int count = 0;
    int index = 0;

    unsigned int vertArrayIndex = 0;
    for (unsigned int faceIdx = 0; faceIdx != mesh->mNumFaces; faceIdx++)
    {
        aiFace face = mesh->mFaces[faceIdx];

        for (int idx = 0; idx != 3; idx++)
        {
            unsigned int index = face.mIndices[idx];

            triangles.push_back(index);


            aiVector3D position = mesh->mVertices[index];
            pTempVertArray[vertArrayIndex].x = position.x;
            pTempVertArray[vertArrayIndex].y = position.y;
            pTempVertArray[vertArrayIndex].z = position.z;
            pTempVertArray[vertArrayIndex].w = 1.0f;

            if (mesh->HasTextureCoords(0))
            {
                aiVector3D textureCoord = mesh->mTextureCoords[0][index];
                pTempVertArray[vertArrayIndex].u = textureCoord.x;
                pTempVertArray[vertArrayIndex].v = textureCoord.y;
                pTempVertArray[vertArrayIndex].u2 = textureCoord.z;
                pTempVertArray[vertArrayIndex].v2 = 0.0f;
            }
            else
            {
                pTempVertArray[vertArrayIndex].u = 0.0f;
                pTempVertArray[vertArrayIndex].v = 0.0f;
                pTempVertArray[vertArrayIndex].u2 = 0.0f;
                pTempVertArray[vertArrayIndex].v2 = 0.0f;
            }

            aiVector3D normal = mesh->mNormals[index];
            pTempVertArray[vertArrayIndex].nx = normal.x;
            pTempVertArray[vertArrayIndex].ny = normal.y;
            pTempVertArray[vertArrayIndex].nz = normal.z;
            pTempVertArray[vertArrayIndex].nw = 0.0f;

            if (mesh->HasBones())
            {
                BoneWeightInfo& boneInfo = boneWeights[index];

                pTempVertArray[vertArrayIndex].bone1 = boneInfo.m_BoneId[0];
                pTempVertArray[vertArrayIndex].bone2 = boneInfo.m_BoneId[1];
                pTempVertArray[vertArrayIndex].bone3 = boneInfo.m_BoneId[2];
                pTempVertArray[vertArrayIndex].bone4 = boneInfo.m_BoneId[3];

                pTempVertArray[vertArrayIndex].bWeight1 = boneInfo.m_Weight[0];
                pTempVertArray[vertArrayIndex].bWeight2 = boneInfo.m_Weight[1];
                pTempVertArray[vertArrayIndex].bWeight3 = boneInfo.m_Weight[2];
                pTempVertArray[vertArrayIndex].bWeight4 = boneInfo.m_Weight[3];

                // float weight = boneInfo.m_Weight[0] + boneInfo.m_Weight[1] + boneInfo.m_Weight[2] + boneInfo.m_Weight[3];

            }


            if (mesh->HasVertexColors(0))
            {
                aiColor4D colour = mesh->mColors[0][index];
                pTempVertArray[vertArrayIndex].r = colour.r;
                pTempVertArray[vertArrayIndex].g = colour.g;
                pTempVertArray[vertArrayIndex].b = colour.b;
                pTempVertArray[vertArrayIndex].a = colour.a;
            }
            else
            {
                pTempVertArray[vertArrayIndex].r = 1.0f;
                pTempVertArray[vertArrayIndex].g = 1.0f;
                pTempVertArray[vertArrayIndex].b = 1.0f;
                pTempVertArray[vertArrayIndex].a = 1.0f;
            }

            if (mesh->HasTangentsAndBitangents())
            {
                aiVector3D tangent = mesh->mTangents[index];
                aiVector3D bitangent = mesh->mBitangents[index];

                pTempVertArray[vertArrayIndex].tx = tangent.x;
                pTempVertArray[vertArrayIndex].ty = tangent.y;
                pTempVertArray[vertArrayIndex].tz = tangent.z;
                pTempVertArray[vertArrayIndex].tw = 0.0f;
                
                pTempVertArray[vertArrayIndex].bx = bitangent.x;
                pTempVertArray[vertArrayIndex].by = bitangent.y;
                pTempVertArray[vertArrayIndex].bz = bitangent.z;
                pTempVertArray[vertArrayIndex].bw = 0.0f;
            }
            else
            {
                pTempVertArray[vertArrayIndex].tx = 0.0f;
                pTempVertArray[vertArrayIndex].ty = 0.0f;
                pTempVertArray[vertArrayIndex].tz = 0.0f;
                pTempVertArray[vertArrayIndex].tw = 0.0f;

                pTempVertArray[vertArrayIndex].bx = 0.0f;
                pTempVertArray[vertArrayIndex].by = 0.0f;
                pTempVertArray[vertArrayIndex].bz = 0.0f;
                pTempVertArray[vertArrayIndex].bw = 0.0f;
            }


            pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;

            vertArrayIndex++;
        }
    }

    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];

    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        drawInfo.pVertices[vertIndex].x = pTempVertArray[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTempVertArray[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTempVertArray[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = pTempVertArray[vertIndex].w;

        drawInfo.pVertices[vertIndex].nx = pTempVertArray[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTempVertArray[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTempVertArray[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = pTempVertArray[vertIndex].nw;

        drawInfo.pVertices[vertIndex].u = pTempVertArray[vertIndex].u;
        drawInfo.pVertices[vertIndex].v = pTempVertArray[vertIndex].v;
        drawInfo.pVertices[vertIndex].u2 = pTempVertArray[vertIndex].u2;
        drawInfo.pVertices[vertIndex].v2 = pTempVertArray[vertIndex].v2;

        drawInfo.pVertices[vertIndex].tx = pTempVertArray[vertIndex].tx;
        drawInfo.pVertices[vertIndex].ty = pTempVertArray[vertIndex].ty;
        drawInfo.pVertices[vertIndex].tz = pTempVertArray[vertIndex].tz;
        drawInfo.pVertices[vertIndex].tw = pTempVertArray[vertIndex].tw;
        
        drawInfo.pVertices[vertIndex].bx = pTempVertArray[vertIndex].bx;
        drawInfo.pVertices[vertIndex].by = pTempVertArray[vertIndex].by;
        drawInfo.pVertices[vertIndex].bz = pTempVertArray[vertIndex].bz;
        drawInfo.pVertices[vertIndex].bw = pTempVertArray[vertIndex].bw;

        /*drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTempVertArray[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTempVertArray[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTempVertArray[vertIndex].a;
        */

        drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].bone1 / 52.0f;
        drawInfo.pVertices[vertIndex].g = 0.0f;//pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].b = 0.0f;//pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].a = 1.0f;//pTempVertArray[vertIndex].bone1;


        drawInfo.pVertices[vertIndex].bone1 = pTempVertArray[vertIndex].bone1;
        drawInfo.pVertices[vertIndex].bone2 = pTempVertArray[vertIndex].bone2;
        drawInfo.pVertices[vertIndex].bone3 = pTempVertArray[vertIndex].bone3;
        drawInfo.pVertices[vertIndex].bone4 = pTempVertArray[vertIndex].bone4;

        drawInfo.pVertices[vertIndex].bWeight1 = pTempVertArray[vertIndex].bWeight1;
        drawInfo.pVertices[vertIndex].bWeight2 = pTempVertArray[vertIndex].bWeight2;
        drawInfo.pVertices[vertIndex].bWeight3 = pTempVertArray[vertIndex].bWeight3;
        drawInfo.pVertices[vertIndex].bWeight4 = pTempVertArray[vertIndex].bWeight4;
    }

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; ++triIndex)
    {
        drawInfo.pIndices[elementIndex + 0] = triangles[elementIndex + 0];
        drawInfo.pIndices[elementIndex + 1] = triangles[elementIndex + 1];
        drawInfo.pIndices[elementIndex + 2] = triangles[elementIndex + 2];

               
        if (mesh->HasNormals() && mesh->HasTextureCoords(0) && !mesh->HasTangentsAndBitangents())
        {
            sVertex vertexOne = drawInfo.pVertices[triangles[elementIndex]];
            sVertex vertexTwo = drawInfo.pVertices[triangles[elementIndex + 1]];
            sVertex vertexThree = drawInfo.pVertices[triangles[elementIndex + 2]];

            glm::vec3 edgeOne = glm::vec3(vertexTwo.x, vertexTwo.y, vertexTwo.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);
            glm::vec3 edgeTwo = glm::vec3(vertexThree.x, vertexThree.y, vertexThree.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);

            glm::vec2 deltaUVOne = glm::vec2(vertexTwo.u, vertexTwo.v) - glm::vec2(vertexOne.u, vertexOne.v);
            glm::vec2 deltaUVTwo = glm::vec2(vertexThree.u, vertexThree.v) - glm::vec2(vertexOne.u, vertexOne.v);

            float fractional = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVTwo.x * deltaUVOne.y);

            glm::vec3 tangent;
            glm::vec3 bitangent;

            tangent.x = fractional * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
            tangent.y = fractional * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
            tangent.z = fractional * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);
            
            bitangent.x = fractional * (-deltaUVTwo.x * edgeOne.x + deltaUVOne.x * edgeTwo.x);
            bitangent.y = fractional * (-deltaUVTwo.x * edgeOne.y + deltaUVOne.x * edgeTwo.y);
            bitangent.z = fractional * (-deltaUVTwo.x * edgeOne.z + deltaUVOne.x * edgeTwo.z);

            tangent = glm::normalize(tangent);
            bitangent = glm::normalize(bitangent);

            
            for (int i = 0; i < 3; i++)
            {
                drawInfo.pVertices[triangles[elementIndex + i]].tx = tangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].ty = tangent.y;
                drawInfo.pVertices[triangles[elementIndex + i]].tz = tangent.z;
                drawInfo.pVertices[triangles[elementIndex + i]].tw = 1.0f;

                drawInfo.pVertices[triangles[elementIndex + i]].bx = bitangent.x;
                drawInfo.pVertices[triangles[elementIndex + i]].by = bitangent.y;
                drawInfo.pVertices[triangles[elementIndex + i]].bz = bitangent.z;
                drawInfo.pVertices[triangles[elementIndex + i]].bw = 1.0f;
            }

        }

        elementIndex += 3;
    }



    // 
    // Model is loaded and the vertices and indices are in the drawInfo struct
    // 

    // Create a VAO (Vertex Array Object), which will 
    //	keep track of all the 'state' needed to draw 
    //	from this buffer...
    glUseProgram(shaderProgramID);
    // Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(drawInfo.VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(drawInfo.VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 


    // NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &(drawInfo.VertexBufferID));

    //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
    // sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
        (GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
        (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));


    // Copy the index buffer into the video card, too
    // Create an index buffer.
    glGenBuffers(1, &(drawInfo.IndexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
        sizeof(unsigned int) * drawInfo.numberOfIndices,
        (GLvoid*)drawInfo.pIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.

    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location = glad_glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
    GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
    GLint vBitangent_location = glGetAttribLocation(shaderProgramID, "vBitangent");
    GLint vBoneIndex_location = glGetAttribLocation(shaderProgramID, "vBoneIndex");
    GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));

    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, nx));

    glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
    glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, u));

    glEnableVertexAttribArray(vTangent_location);	    // vTangent;
    glVertexAttribPointer(vTangent_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, tx));
    
    glEnableVertexAttribArray(vBitangent_location);	    // vBoneIndex;
    glVertexAttribPointer(vBitangent_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bx));
    
    glEnableVertexAttribArray(vBoneIndex_location);	    // vBoneIndex;
    glVertexAttribPointer(vBoneIndex_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bone1));

    glEnableVertexAttribArray(vBoneWeight_location);	    // vBoneWeight;
    glVertexAttribPointer(vBoneWeight_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, bWeight1));

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDisableVertexAttribArray(vNormal_location);
    glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
    glDisableVertexAttribArray(vTangent_location);        // <-- New
    glDisableVertexAttribArray(vBitangent_location);        // <-- New
    glDisableVertexAttribArray(vBoneIndex_location);        // <-- New
    glDisableVertexAttribArray(vBoneWeight_location);        // <-- New

    
    
    // Store the draw information into the map
    this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;
    

    return true;
}

//bool cVAOManager::LoadModelWithAssimp(std::string fileName,
//    sModelDrawInfo& drawInfo,
//    unsigned int shaderProgramID,
//    bool bIsDynamicBuffer /*=false*/)
//{
//    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;
//    drawInfo.meshName = fileName;
//
//    aiScene* scene = (aiScene*)m_AssimpImporter.ReadFile(fileAndPath, aiProcess_GenNormals);//aiProcess_GenSmoothNormals);
//
//    if (scene == 0 || !scene->HasMeshes())
//    {
//        return false;
//    }
//
//    aiMesh* mesh = scene->mMeshes[0];
//
//    if (scene->mNumAnimations > 0)
//    {
//        
//        CharacterAnimation* characterAnimation = new CharacterAnimation();
//        aiAnimation* animation = scene->mAnimations[0];
//
//        characterAnimation->Name = animation->mName.C_Str();
//        characterAnimation->duration = animation->mDuration;
//        characterAnimation->ticksPerSecond = animation->mTicksPerSecond;
//        characterAnimation->currentTime = 0.0;
//
//        for (int i = 0; i < animation->mNumChannels; i++)
//        {
//            aiNodeAnim* assimpNodeAnim = animation->mChannels[i];
//            NodeAnim* nodeAnim = new NodeAnim(assimpNodeAnim->mNodeName.C_Str());
//
//            for (int j = 0; j < assimpNodeAnim->mNumPositionKeys; ++j)
//            {
//                aiVectorKey& p = assimpNodeAnim->mPositionKeys[j];
//                nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(glm::vec3(p.mValue.x, p.mValue.y, p.mValue.z), p.mTime));
//            }
//            for (int j = 0; j < assimpNodeAnim->mNumScalingKeys; ++j)
//            {
//                aiVectorKey& s = assimpNodeAnim->mScalingKeys[j];
//                nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(glm::vec3(s.mValue.x, s.mValue.y, s.mValue.z), s.mTime));
//            }
//            for (int j = 0; j < assimpNodeAnim->mNumRotationKeys; ++j)
//            {
//                aiQuatKey& q = assimpNodeAnim->mRotationKeys[j];
//                nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(glm::quat(q.mValue.w, q.mValue.x, q.mValue.y, q.mValue.z), q.mTime));
//            }
//
//            characterAnimation->Channels.emplace_back(nodeAnim);
//        }
//        
//        drawInfo.CharacterAnimations.push_back(characterAnimation);
//    }
//
//    
//    drawInfo.RootNode = drawInfo.GenerateBoneHierarchy(scene->mRootNode);
//    drawInfo.GlobalInverseTransformation = glm::inverse(drawInfo.RootNode->transformation);
//
//    std::vector<BoneWeightInfo> boneWeights;
//
//    if (mesh->HasBones())
//    {
//        boneWeights.resize(mesh->mNumVertices);
//        unsigned int numBones = mesh->mNumBones;
//
//        for (unsigned int boneIdx = 0; boneIdx < numBones; ++boneIdx)
//        {
//            aiBone* bone = mesh->mBones[boneIdx];
//
//            std::string name(bone->mName.C_Str(), bone->mName.length);
//            if (drawInfo.BoneNameToIdMap.find(bone->mName.C_Str()) == drawInfo.BoneNameToIdMap.end())
//            {
//                drawInfo.BoneNameToIdMap.insert(std::pair<std::string, int>(name, drawInfo.BoneInfoVec.size()));
//            }
//
//            // store offset matrices
//            BoneInfo info;
//            drawInfo.AssimpToGLM(bone->mOffsetMatrix, info.BoneOffset);
//            drawInfo.BoneInfoVec.emplace_back(info);
//
//
//            for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
//            {
//                aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
//                // BoneId		:	boneIdx
//                // Vertexid		:	vertexWeight.mVertexId
//                // Weight		:	vertexWeight.mWeight
//
//
//
//                BoneWeightInfo& boneInfo = boneWeights[vertexWeight.mVertexId];
//                for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
//                {
//                    if (boneInfo.m_Weight[infoIdx] == 0.0f)
//                    {
//                        boneInfo.m_BoneId[infoIdx] = boneIdx;
//                        boneInfo.m_Weight[infoIdx] = vertexWeight.mWeight;
//                        break;
//                    }
//                }
//            }
//        }
//    }
//
//    unsigned int numVerticesInVertArray = mesh->mNumVertices;
//    unsigned int numIndicesInIndexArray = mesh->mNumFaces * 3;
//
//    drawInfo.numberOfVertices = numVerticesInVertArray;
//    drawInfo.numberOfTriangles = mesh->mNumFaces;
//    drawInfo.numberOfIndices = numIndicesInIndexArray;
//    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];
//
//    std::vector<unsigned int>triangles;
//
//    sVertex* pTempVertArray = new sVertex[numIndicesInIndexArray * 2];
//
//    GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray * 2];
//
//    int count = 0;
//    int index = 0;
//
//    unsigned int vertArrayIndex = 0;
//    for (unsigned int faceIdx = 0; faceIdx != mesh->mNumFaces; faceIdx++)
//    {
//        aiFace face = mesh->mFaces[faceIdx];
//
//        for (int idx = 0; idx != 3; idx++)
//        {
//            unsigned int index = face.mIndices[idx];
//
//            triangles.push_back(index);
//
//
//            aiVector3D position = mesh->mVertices[index];
//            pTempVertArray[vertArrayIndex].x = position.x;
//            pTempVertArray[vertArrayIndex].y = position.y;
//            pTempVertArray[vertArrayIndex].z = position.z;
//            pTempVertArray[vertArrayIndex].w = 1.0f;
//
//            if (mesh->HasTextureCoords(0))
//            {
//                aiVector3D textureCoord = mesh->mTextureCoords[0][index];
//                pTempVertArray[vertArrayIndex].u = textureCoord.x;
//                pTempVertArray[vertArrayIndex].v = textureCoord.y;
//                pTempVertArray[vertArrayIndex].u2 = textureCoord.z;
//                pTempVertArray[vertArrayIndex].v2 = 0.0f;
//            }
//            else
//            {
//                pTempVertArray[vertArrayIndex].u = 0.0f;
//                pTempVertArray[vertArrayIndex].v = 0.0f;
//                pTempVertArray[vertArrayIndex].u2 = 0.0f;
//                pTempVertArray[vertArrayIndex].v2 = 0.0f;
//            }
//
//            aiVector3D normal = mesh->mNormals[index];
//            pTempVertArray[vertArrayIndex].nx = normal.x;
//            pTempVertArray[vertArrayIndex].ny = normal.y;
//            pTempVertArray[vertArrayIndex].nz = normal.z;
//            pTempVertArray[vertArrayIndex].nw = 0.0f;
//
//            if (mesh->HasBones())
//            {
//                BoneWeightInfo& boneInfo = boneWeights[index];
//
//                pTempVertArray[vertArrayIndex].bone1 = boneInfo.m_BoneId[0];
//                pTempVertArray[vertArrayIndex].bone2 = boneInfo.m_BoneId[1];
//                pTempVertArray[vertArrayIndex].bone3 = boneInfo.m_BoneId[2];
//                pTempVertArray[vertArrayIndex].bone4 = boneInfo.m_BoneId[3];
//
//                pTempVertArray[vertArrayIndex].bWeight1 = boneInfo.m_Weight[0];
//                pTempVertArray[vertArrayIndex].bWeight2 = boneInfo.m_Weight[1];
//                pTempVertArray[vertArrayIndex].bWeight3 = boneInfo.m_Weight[2];
//                pTempVertArray[vertArrayIndex].bWeight4 = boneInfo.m_Weight[3];
//
//                // float weight = boneInfo.m_Weight[0] + boneInfo.m_Weight[1] + boneInfo.m_Weight[2] + boneInfo.m_Weight[3];
//
//            }
//
//
//            if (mesh->HasVertexColors(0))
//            {
//                aiColor4D colour = mesh->mColors[0][index];
//                pTempVertArray[vertArrayIndex].r = colour.r;
//                pTempVertArray[vertArrayIndex].g = colour.g;
//                pTempVertArray[vertArrayIndex].b = colour.b;
//                pTempVertArray[vertArrayIndex].a = colour.a;
//            }
//            else
//            {
//                pTempVertArray[vertArrayIndex].r = 1.0f;
//                pTempVertArray[vertArrayIndex].g = 1.0f;
//                pTempVertArray[vertArrayIndex].b = 1.0f;
//                pTempVertArray[vertArrayIndex].a = 1.0f;
//            }
//
//            pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;
//
//            vertArrayIndex++;
//        }
//    }
//
//    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
//
//    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
//    {
//        drawInfo.pVertices[vertIndex].x = pTempVertArray[vertIndex].x;
//        drawInfo.pVertices[vertIndex].y = pTempVertArray[vertIndex].y;
//        drawInfo.pVertices[vertIndex].z = pTempVertArray[vertIndex].z;
//        drawInfo.pVertices[vertIndex].w = pTempVertArray[vertIndex].w;
//
//        drawInfo.pVertices[vertIndex].nx = pTempVertArray[vertIndex].nx;
//        drawInfo.pVertices[vertIndex].ny = pTempVertArray[vertIndex].ny;
//        drawInfo.pVertices[vertIndex].nz = pTempVertArray[vertIndex].nz;
//        drawInfo.pVertices[vertIndex].nw = pTempVertArray[vertIndex].nw;
//
//        drawInfo.pVertices[vertIndex].u = pTempVertArray[vertIndex].u;
//        drawInfo.pVertices[vertIndex].v = pTempVertArray[vertIndex].v;
//        drawInfo.pVertices[vertIndex].u2 = pTempVertArray[vertIndex].u2;
//        drawInfo.pVertices[vertIndex].v2 = pTempVertArray[vertIndex].v2;
//
//        /*drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].r;
//        drawInfo.pVertices[vertIndex].g = pTempVertArray[vertIndex].g;
//        drawInfo.pVertices[vertIndex].b = pTempVertArray[vertIndex].b;
//        drawInfo.pVertices[vertIndex].a = pTempVertArray[vertIndex].a;
//        */
//
//        drawInfo.pVertices[vertIndex].r = pTempVertArray[vertIndex].bone1 / 52.0f;
//        drawInfo.pVertices[vertIndex].g = 0.0f;//pTempVertArray[vertIndex].bone1;
//        drawInfo.pVertices[vertIndex].b = 0.0f;//pTempVertArray[vertIndex].bone1;
//        drawInfo.pVertices[vertIndex].a = 1.0f;//pTempVertArray[vertIndex].bone1;
//
//
//        drawInfo.pVertices[vertIndex].bone1 = pTempVertArray[vertIndex].bone1;
//        drawInfo.pVertices[vertIndex].bone2 = pTempVertArray[vertIndex].bone2;
//        drawInfo.pVertices[vertIndex].bone3 = pTempVertArray[vertIndex].bone3;
//        drawInfo.pVertices[vertIndex].bone4 = pTempVertArray[vertIndex].bone4;
//
//        drawInfo.pVertices[vertIndex].bWeight1 = pTempVertArray[vertIndex].bWeight1;
//        drawInfo.pVertices[vertIndex].bWeight2 = pTempVertArray[vertIndex].bWeight2;
//        drawInfo.pVertices[vertIndex].bWeight3 = pTempVertArray[vertIndex].bWeight3;
//        drawInfo.pVertices[vertIndex].bWeight4 = pTempVertArray[vertIndex].bWeight4;
//    }
//
//    unsigned int elementIndex = 0;
//    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; ++triIndex)
//    {
//        drawInfo.pIndices[elementIndex + 0] = triangles[elementIndex + 0];
//        drawInfo.pIndices[elementIndex + 1] = triangles[elementIndex + 1];
//        drawInfo.pIndices[elementIndex + 2] = triangles[elementIndex + 2];
//
//        elementIndex += 3;
//    }
//
//
//
//    // 
//    // Model is loaded and the vertices and indices are in the drawInfo struct
//    // 
//
//    // Create a VAO (Vertex Array Object), which will 
//    //	keep track of all the 'state' needed to draw 
//    //	from this buffer...
//
//    // Ask OpenGL for a new buffer ID...
//    glGenVertexArrays(1, &(drawInfo.VAO_ID));
//    // "Bind" this buffer:
//    // - aka "make this the 'current' VAO buffer
//    glBindVertexArray(drawInfo.VAO_ID);
//
//    // Now ANY state that is related to vertex or index buffer
//    //	and vertex attribute layout, is stored in the 'state' 
//    //	of the VAO... 
//
//
//    // NOTE: OpenGL error checks have been omitted for brevity
////	glGenBuffers(1, &vertex_buffer);
//    glGenBuffers(1, &(drawInfo.VertexBufferID));
//
//    //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
//    // sVert vertices[3]
//
//    // This is updated for bIsDynamicBuffer so:
//    // * if true, then it's GL_DYNAMIC_DRAW
//    // * if false, then it's GL_STATIC_DRAW
//    // 
//    // Honestly, it's not that Big Of A Deal in that you can still update 
//    //  a buffer if it's set to STATIC, but in theory this will take longer.
//    // Does it really take longer? Who knows?
//    glBufferData(GL_ARRAY_BUFFER,
//        sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
//        (GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
//        (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
//
//
//    // Copy the index buffer into the video card, too
//    // Create an index buffer.
//    glGenBuffers(1, &(drawInfo.IndexBufferID));
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);
//
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
//        sizeof(unsigned int) * drawInfo.numberOfIndices,
//        (GLvoid*)drawInfo.pIndices,
//        GL_STATIC_DRAW);
//
//    // Set the vertex attributes.
//
//    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
//    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
//    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
//    // AND the texture coordinate
//    GLint vTextureCoords_location = glad_glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
//    GLint vBoneIndex_location = glGetAttribLocation(shaderProgramID, "vBoneIndex");
//    GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");
//
//    // Set the vertex attributes for this shader
//    glEnableVertexAttribArray(vpos_location);	    // vPos
//    glVertexAttribPointer(vpos_location, 4,		// vPos
//        GL_FLOAT, GL_FALSE,
//        sizeof(sVertex),
//        (void*)offsetof(sVertex, x));
//
//    glEnableVertexAttribArray(vcol_location);	    // vCol
//    glVertexAttribPointer(vcol_location, 4,		// vCol
//        GL_FLOAT, GL_FALSE,
//        sizeof(sVertex),
//        (void*)offsetof(sVertex, r));
//
//    glEnableVertexAttribArray(vNormal_location);	// vNormal
//    glVertexAttribPointer(vNormal_location, 4,		// vNormal
//        GL_FLOAT, GL_FALSE,
//        sizeof(sVertex),
//        (void*)offsetof(sVertex, nx));
//
//    glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
//    glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
//        GL_FLOAT, GL_FALSE,
//        sizeof(sVertex),
//        (void*)offsetof(sVertex, u));
//
//    glEnableVertexAttribArray(vBoneIndex_location);	    // vBoneIndex;
//    glVertexAttribPointer(vBoneIndex_location, 4,
//        GL_FLOAT, GL_FALSE,
//        sizeof(sVertex),
//        (void*)offsetof(sVertex, bone1));
//
//    glEnableVertexAttribArray(vBoneWeight_location);	    // vBoneWeight;
//    glVertexAttribPointer(vBoneWeight_location, 4,
//        GL_FLOAT, GL_FALSE,
//        sizeof(sVertex),
//        (void*)offsetof(sVertex, bWeight1));
//
//    // Now that all the parts are set up, set the VAO to zero
//    glBindVertexArray(0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//    glDisableVertexAttribArray(vpos_location);
//    glDisableVertexAttribArray(vcol_location);
//    glDisableVertexAttribArray(vNormal_location);
//    glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
//    glDisableVertexAttribArray(vBoneIndex_location);        // <-- New
//    glDisableVertexAttribArray(vBoneWeight_location);        // <-- New
//
//
//    // Store the draw information into the map
//    this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;
//    
//
//    return true;
//}

bool cVAOManager::LoadModelIntoVAOWithBones(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID,
        bool bIsDynamicBuffer /*=false*/)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;

//    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA(fileAndPath, drawInfo) )
   // if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA_UV(fileAndPath, drawInfo) )
    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA_UV_ManualBone(fileAndPath, drawInfo) ) 
    {
        return false;
    };


	// ***********************************************************
	// TODO: Load the model from file

	// ***********************************************************
	// 
	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location = glad_glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
    GLint vBoneIndex_location = glGetAttribLocation(shaderProgramID, "vBoneIndex");
    GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight"); 

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	    // vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVertex), 
						   ( void* ) offsetof(sVertex, x));

	glEnableVertexAttribArray(vcol_location);	    // vCol
	glVertexAttribPointer( vcol_location, 4,		// vCol
						   GL_FLOAT, GL_FALSE,
                          sizeof(sVertex),
						   ( void* ) offsetof(sVertex, r));

	glEnableVertexAttribArray(vNormal_location);	// vNormal
	glVertexAttribPointer(vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, nx));

	glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
	glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, u));
    
    glEnableVertexAttribArray(vBoneIndex_location);	    // vBoneIndex;
	glVertexAttribPointer(vBoneIndex_location, 4,		
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, bone1)); 
    
    glEnableVertexAttribArray(vBoneWeight_location);	    // vBoneWeight;
	glVertexAttribPointer(vBoneWeight_location, 4,		
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, bWeight1)); 

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
    glDisableVertexAttribArray(vBoneIndex_location);        // <-- New
    glDisableVertexAttribArray(vBoneWeight_location);        // <-- New


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}

bool cVAOManager::LoadModelIntoVAOWithManualBone(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID,
        bool bIsDynamicBuffer /*=false*/)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;

//    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA(fileAndPath, drawInfo) )
   // if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA_UV(fileAndPath, drawInfo) )
    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA_UV_ManualBone(fileAndPath, drawInfo) ) 
    {
        return false;
    };


	// ***********************************************************
	// TODO: Load the model from file

	// ***********************************************************
	// 
	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location = glad_glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
    GLint vBoneIndex = glGetAttribLocation(shaderProgramID, "vBoneIndex");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	    // vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVertex), 
						   ( void* ) offsetof(sVertex, x));

	glEnableVertexAttribArray(vcol_location);	    // vCol
	glVertexAttribPointer( vcol_location, 4,		// vCol
						   GL_FLOAT, GL_FALSE,
                          sizeof(sVertex),
						   ( void* ) offsetof(sVertex, r));

	glEnableVertexAttribArray(vNormal_location);	// vNormal
	glVertexAttribPointer(vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, nx));

	glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
	glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, u));
    
    glEnableVertexAttribArray(vBoneIndex);	    // vBoneIndex;
	glVertexAttribPointer(vBoneIndex, 4,		// in vec2 vTextureCoords;
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, bone1)); 

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
    glDisableVertexAttribArray(vBoneIndex);        // <-- New


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}

bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID,
        bool bIsDynamicBuffer /*=false*/)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;

//    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA(fileAndPath, drawInfo) )
    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA_UV(fileAndPath, drawInfo) )
    //if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA_UV_ManualBone(fileAndPath, drawInfo) ) 
    {
        return false;
    };


	// ***********************************************************
	// TODO: Load the model from file

	// ***********************************************************
	// 
	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    // AND the texture coordinate
    GLint vTextureCoords_location = glad_glGetAttribLocation(shaderProgramID, "vTextureCoords");    // in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4;		// NOTE this is a vec2 not vec4
   
	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	    // vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVertex), 
						   ( void* ) offsetof(sVertex, x));

	glEnableVertexAttribArray(vcol_location);	    // vCol
	glVertexAttribPointer( vcol_location, 4,		// vCol
						   GL_FLOAT, GL_FALSE,
                          sizeof(sVertex),
						   ( void* ) offsetof(sVertex, r));

	glEnableVertexAttribArray(vNormal_location);	// vNormal
	glVertexAttribPointer(vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, nx));

	glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
	glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, u));
    
   

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vTextureCoords_location);        // <-- New
   

	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}



bool cVAOManager::m_LoadTheFile_Ply_XYZ_N_RGBA(std::string theFileName, sModelDrawInfo& drawInfo)
{

//    ply
//    format ascii 1.0
//    comment VCGLIB generated
//    element vertex 642
//    property float x
//    property float y
//    property float z
//    property float nx
//    property float ny
//    property float nz
//    property uchar red
//    property uchar green
//    property uchar blue
//    property uchar alpha



    //sVertexPlyFile p;       p.x = 0.0f;     p.y = 1.0f; p.z = 2.0f;
    //std::cout << p.x;

    //sVertexPlyFile q;
    //std::cout << "Type in the x: ";
    //std::cin >> q.x;

    // Input Filestream 
//    std::ifstream theBunnyFile("bathtub.ply");
    std::ifstream theBunnyFile( theFileName.c_str() );
    if (!theBunnyFile.is_open())
    {
        // didn't open the file.
        return false;
    }

    std::string temp;
    while (theBunnyFile >> temp)
    {
        if (temp == "vertex")
        {
            break;
        }
    };
    //element vertex 8171
//    theBunnyFile >> g_numberOfVertices;
    theBunnyFile >> drawInfo.numberOfVertices;


    while (theBunnyFile >> temp)
    {
        if (temp == "face")
        {
            break;
        }
    };
    //element vertex 8171
//    theBunnyFile >> g_numberOfTriangles;
    theBunnyFile >> drawInfo.numberOfTriangles;

    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

    while (theBunnyFile >> temp)
    {
        if (temp == "end_header")
        {
            break;
        }
    };


    // Allocate enough space to hold the vertices
//    sVertex vertices[8171];                 // Stack

//    sVertex x;      // STACK based variable (on the stack)
//    sVertex* px;    // Pointer variable.

//    int y = 5;
//
//    int* py = new int();
//    *py = 5;



    // This most closely matches the ply file for the bunny
    struct sVertexPlyFile
    {
        float x;
        float y;
        float z;
        float nx, ny, nz;
        float r, g, b, a;
    };

    struct sTrianglePlyFile
    {
    //    sVertexPlyFile verts[3];
    //    unsigned int vertIndexes[3];
        // Vertices of the triangles
        unsigned int v0, v1, v2;
    };

    // Dynamically allocate memory on the heap;
//    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[g_numberOfVertices];
    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[drawInfo.numberOfVertices];

    // -0.036872 0.127727 0.00440925 
//    for (unsigned int index = 0; index != g_numberOfVertices; index++)
    for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
    {
        sVertexPlyFile tempVertex;
        theBunnyFile >> tempVertex.x;                //std::cin >> a.x;
        theBunnyFile >> tempVertex.y;                //std::cin >> a.y;
        theBunnyFile >> tempVertex.z;                //std::cin >> a.z;

        theBunnyFile >> tempVertex.nx;
        theBunnyFile >> tempVertex.ny;
        theBunnyFile >> tempVertex.nz;

        theBunnyFile >> tempVertex.r;       tempVertex.r /= 255.0f;
        theBunnyFile >> tempVertex.g;       tempVertex.g /= 255.0f;
        theBunnyFile >> tempVertex.b;       tempVertex.b /= 255.0f;
        theBunnyFile >> tempVertex.a;       tempVertex.a /= 255.0f;


        pTheVerticesFile[index] = tempVertex;
    }


//    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[g_numberOfTriangles];
    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[drawInfo.numberOfTriangles];

    // 3 3495 3549 3548 
    for (unsigned int index = 0; index != drawInfo.numberOfTriangles; index++)
    {
        sTrianglePlyFile tempTriangle;

        unsigned int discard;
        theBunnyFile >> discard;            // 3
        theBunnyFile >> tempTriangle.v0;                //std::cin >> a.x;
        theBunnyFile >> tempTriangle.v1;                //std::cin >> a.y;
        theBunnyFile >> tempTriangle.v2;                //std::cin >> a.z;

        pTheTriangles[index] = tempTriangle;
    }

    // Before, we manually looked up the vertex indices and added them to the vertex buffer
    //g_NumberOfVerticesToDraw = g_numberOfTriangles * 3;
    //pVertices = new sVertex[g_NumberOfVerticesToDraw];

    //unsigned int vertIndex = 0;
    //for (unsigned int triIndex = 0; triIndex != g_numberOfTriangles; triIndex++)
    //{
    //    // 3 1582 1581 2063 
    //    pVertices[vertIndex + 0].x = pTheVerticesFile[pTheTriangles[triIndex].v0].x;
    //    pVertices[vertIndex + 0].y = pTheVerticesFile[pTheTriangles[triIndex].v0].y;
    //    pVertices[vertIndex + 0].z = pTheVerticesFile[pTheTriangles[triIndex].v0].z;

    //    pVertices[vertIndex + 0].r = pTheVerticesFile[pTheTriangles[triIndex].v0].r;
    //    pVertices[vertIndex + 0].g = pTheVerticesFile[pTheTriangles[triIndex].v0].g;
    //    pVertices[vertIndex + 0].b = pTheVerticesFile[pTheTriangles[triIndex].v0].b;

    //    pVertices[vertIndex + 1].x = pTheVerticesFile[pTheTriangles[triIndex].v1].x;
    //    pVertices[vertIndex + 1].y = pTheVerticesFile[pTheTriangles[triIndex].v1].y;
    //    pVertices[vertIndex + 1].z = pTheVerticesFile[pTheTriangles[triIndex].v1].z;

    //    pVertices[vertIndex + 1].r = pTheVerticesFile[pTheTriangles[triIndex].v1].r;
    //    pVertices[vertIndex + 1].g = pTheVerticesFile[pTheTriangles[triIndex].v1].g;
    //    pVertices[vertIndex + 1].b = pTheVerticesFile[pTheTriangles[triIndex].v1].b;

    //    pVertices[vertIndex + 2].x = pTheVerticesFile[pTheTriangles[triIndex].v2].x;
    //    pVertices[vertIndex + 2].y = pTheVerticesFile[pTheTriangles[triIndex].v2].y;
    //    pVertices[vertIndex + 2].z = pTheVerticesFile[pTheTriangles[triIndex].v2].z;

    //    pVertices[vertIndex + 2].r = pTheVerticesFile[pTheTriangles[triIndex].v2].r;
    //    pVertices[vertIndex + 2].g = pTheVerticesFile[pTheTriangles[triIndex].v2].g;
    //    pVertices[vertIndex + 2].b = pTheVerticesFile[pTheTriangles[triIndex].v2].b;

    //    vertIndex += 3;
    //}

    // ... now we just copy the vertices from the file as is (unchanged)
    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pVertices[vertIndex].x = pTheVerticesFile[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTheVerticesFile[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTheVerticesFile[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = 1.0f;


        drawInfo.pVertices[vertIndex].nx = pTheVerticesFile[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTheVerticesFile[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTheVerticesFile[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = 1.0f;

        drawInfo.pVertices[vertIndex].r = pTheVerticesFile[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTheVerticesFile[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTheVerticesFile[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTheVerticesFile[vertIndex].a;
    }

    // Allocate an array for all the indices (which is 3x the number of triangles)
    // Element array is an 1D array of integers
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; triIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pIndices[elementIndex + 0] = pTheTriangles[triIndex].v0;
        drawInfo.pIndices[elementIndex + 1] = pTheTriangles[triIndex].v1;
        drawInfo.pIndices[elementIndex + 2] = pTheTriangles[triIndex].v2;

        elementIndex += 3;      // Next "triangle"
    }


    return true;
}

bool cVAOManager::m_LoadTheFile_Ply_XYZ_N_RGBA_UV_ManualBone(std::string theFileName, sModelDrawInfo& drawInfo)
{
//    ply
//    format ascii 1.0
//    element vertex 66049
//    property float x
//    property float y
//    property float z
//    property float nx
//    property float ny
//    property float nz
//    property uchar red
//    property uchar green
//    property uchar blue
//    property uchar alpha
//    property float texture_u          <--- NEW!
//    property float texture_v          <--- NEW!

    //sVertexPlyFile p;       p.x = 0.0f;     p.y = 1.0f; p.z = 2.0f;
    //std::cout << p.x;

    //sVertexPlyFile q;
    //std::cout << "Type in the x: ";
    //std::cin >> q.x;

    // Input Filestream 
//    std::ifstream theBunnyFile("bathtub.ply");
    std::ifstream theBunnyFile( theFileName.c_str() );
    if (!theBunnyFile.is_open())
    {
        // didn't open the file.
        return false;
    }

    std::string temp;
    while (theBunnyFile >> temp)
    {
        if (temp == "vertex")
        {
            break;
        }
    };
    //element vertex 8171
//    theBunnyFile >> g_numberOfVertices;
    theBunnyFile >> drawInfo.numberOfVertices;


    while (theBunnyFile >> temp)
    {
        if (temp == "face")
        {
            break;
        }
    };
    //element vertex 8171
//    theBunnyFile >> g_numberOfTriangles;
    theBunnyFile >> drawInfo.numberOfTriangles;

    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

    while (theBunnyFile >> temp)
    {
        if (temp == "end_header")
        {
            break;
        }
    };


    // Allocate enough space to hold the vertices
//    sVertex vertices[8171];                 // Stack

//    sVertex x;      // STACK based variable (on the stack)
//    sVertex* px;    // Pointer variable.

//    int y = 5;
//
//    int* py = new int();
//    *py = 5;



    // This most closely matches the ply file for the bunny
    struct sVertexPlyFile
    {
        float x, y, z;
        float nx, ny, nz;
        float r, g, b, a;
        float u, v;             // <--- NEW!!!
       // float bone1, bone2, bone3, bone4;
    };

    struct sTrianglePlyFile
    {
    //    sVertexPlyFile verts[3];
    //    unsigned int vertIndexes[3];
        // Vertices of the triangles
        unsigned int v0, v1, v2;
    };

    // Dynamically allocate memory on the heap;
//    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[g_numberOfVertices];
    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[drawInfo.numberOfVertices];

    // -0.036872 0.127727 0.00440925 
//    for (unsigned int index = 0; index != g_numberOfVertices; index++)
    for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
    {
        sVertexPlyFile tempVertex;
        theBunnyFile >> tempVertex.x;                //std::cin >> a.x;
        theBunnyFile >> tempVertex.y;                //std::cin >> a.y;
        theBunnyFile >> tempVertex.z;                //std::cin >> a.z;

        theBunnyFile >> tempVertex.nx;
        theBunnyFile >> tempVertex.ny;
        theBunnyFile >> tempVertex.nz;

        theBunnyFile >> tempVertex.r;       tempVertex.r /= 255.0f;
        theBunnyFile >> tempVertex.g;       tempVertex.g /= 255.0f;
        theBunnyFile >> tempVertex.b;       tempVertex.b /= 255.0f;
        theBunnyFile >> tempVertex.a;       tempVertex.a /= 255.0f;

        // ALSO load the UV coordinates
        theBunnyFile >> tempVertex.u;
        theBunnyFile >> tempVertex.v;



        pTheVerticesFile[index] = tempVertex;
    }


//    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[g_numberOfTriangles];
    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[drawInfo.numberOfTriangles];

    // 3 3495 3549 3548 
    for (unsigned int index = 0; index != drawInfo.numberOfTriangles; index++)
    {
        sTrianglePlyFile tempTriangle;

        unsigned int discard;
        theBunnyFile >> discard;            // 3
        theBunnyFile >> tempTriangle.v0;                //std::cin >> a.x;
        theBunnyFile >> tempTriangle.v1;                //std::cin >> a.y;
        theBunnyFile >> tempTriangle.v2;                //std::cin >> a.z;

        pTheTriangles[index] = tempTriangle;
    }

    /*// Before, we manually looked up the vertex indices and added them to the vertex buffer
    //g_NumberOfVerticesToDraw = g_numberOfTriangles * 3;
    //pVertices = new sVertex[g_NumberOfVerticesToDraw];

    //unsigned int vertIndex = 0;
    //for (unsigned int triIndex = 0; triIndex != g_numberOfTriangles; triIndex++)
    //{
    //    // 3 1582 1581 2063 
    //    pVertices[vertIndex + 0].x = pTheVerticesFile[pTheTriangles[triIndex].v0].x;
    //    pVertices[vertIndex + 0].y = pTheVerticesFile[pTheTriangles[triIndex].v0].y;
    //    pVertices[vertIndex + 0].z = pTheVerticesFile[pTheTriangles[triIndex].v0].z;

    //    pVertices[vertIndex + 0].r = pTheVerticesFile[pTheTriangles[triIndex].v0].r;
    //    pVertices[vertIndex + 0].g = pTheVerticesFile[pTheTriangles[triIndex].v0].g;
    //    pVertices[vertIndex + 0].b = pTheVerticesFile[pTheTriangles[triIndex].v0].b;

    //    pVertices[vertIndex + 1].x = pTheVerticesFile[pTheTriangles[triIndex].v1].x;
    //    pVertices[vertIndex + 1].y = pTheVerticesFile[pTheTriangles[triIndex].v1].y;
    //    pVertices[vertIndex + 1].z = pTheVerticesFile[pTheTriangles[triIndex].v1].z;

    //    pVertices[vertIndex + 1].r = pTheVerticesFile[pTheTriangles[triIndex].v1].r;
    //    pVertices[vertIndex + 1].g = pTheVerticesFile[pTheTriangles[triIndex].v1].g;
    //    pVertices[vertIndex + 1].b = pTheVerticesFile[pTheTriangles[triIndex].v1].b;

    //    pVertices[vertIndex + 2].x = pTheVerticesFile[pTheTriangles[triIndex].v2].x;
    //    pVertices[vertIndex + 2].y = pTheVerticesFile[pTheTriangles[triIndex].v2].y;
    //    pVertices[vertIndex + 2].z = pTheVerticesFile[pTheTriangles[triIndex].v2].z;

    //    pVertices[vertIndex + 2].r = pTheVerticesFile[pTheTriangles[triIndex].v2].r;
    //    pVertices[vertIndex + 2].g = pTheVerticesFile[pTheTriangles[triIndex].v2].g;
    //    pVertices[vertIndex + 2].b = pTheVerticesFile[pTheTriangles[triIndex].v2].b;

    //    vertIndex += 3;
    //}*/

    // ... now we just copy the vertices from the file as is (unchanged)
    std::vector<int> boneCount;
    boneCount.resize(5, 0);
    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pVertices[vertIndex].x = pTheVerticesFile[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTheVerticesFile[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTheVerticesFile[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = 1.0f;

        drawInfo.pVertices[vertIndex].nx = pTheVerticesFile[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTheVerticesFile[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTheVerticesFile[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = 1.0f;

        drawInfo.pVertices[vertIndex].r = pTheVerticesFile[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTheVerticesFile[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTheVerticesFile[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTheVerticesFile[vertIndex].a;

        if (drawInfo.pVertices[vertIndex].y < 1.0f)
        {
            drawInfo.pVertices[vertIndex].bone1 = 0;
            drawInfo.pVertices[vertIndex].r = 255.0f;
            drawInfo.pVertices[vertIndex].g = 0.0f;
            drawInfo.pVertices[vertIndex].b = 0.0f;
            drawInfo.pVertices[vertIndex].a = 255.0f;
            boneCount[0]++;
        }
        else if (drawInfo.pVertices[vertIndex].y < 2.0f)
        {
            drawInfo.pVertices[vertIndex].bone1 = 1;
            drawInfo.pVertices[vertIndex].r = 255.0f;
            drawInfo.pVertices[vertIndex].g = 255.0f;
            drawInfo.pVertices[vertIndex].b = 0.0f;
            drawInfo.pVertices[vertIndex].a = 255.0f;
            boneCount[1]++;
        }
        else if (drawInfo.pVertices[vertIndex].y < 3.0f)
        {
            drawInfo.pVertices[vertIndex].bone1 = 2;
            drawInfo.pVertices[vertIndex].r = 0.0f;
            drawInfo.pVertices[vertIndex].g = 255.0f;
            drawInfo.pVertices[vertIndex].b = 0.0f;
            drawInfo.pVertices[vertIndex].a = 255.0f;
            boneCount[2]++;
        }
        else if (drawInfo.pVertices[vertIndex].y < 4.0f)
        {
            drawInfo.pVertices[vertIndex].bone1 = 3;
            drawInfo.pVertices[vertIndex].r = 0.0f;
            drawInfo.pVertices[vertIndex].g = 0.0f;
            drawInfo.pVertices[vertIndex].b = 255.0f;
            drawInfo.pVertices[vertIndex].a = 255.0f;
            boneCount[3]++;
        }
        else
        {
            drawInfo.pVertices[vertIndex].bone1 = 1;
            drawInfo.pVertices[vertIndex].r = 255.0f;
            drawInfo.pVertices[vertIndex].g = 155.0f;
            drawInfo.pVertices[vertIndex].b = 0.0f;
            drawInfo.pVertices[vertIndex].a = 255.0f;
            boneCount[4]++;
        }

        // Copy the UV coords to the array that will eventually be copied to the GPU
        drawInfo.pVertices[vertIndex].u = pTheVerticesFile[vertIndex].u;
        drawInfo.pVertices[vertIndex].v = pTheVerticesFile[vertIndex].v;

    }

    for (int i = 0; i < boneCount.size(); i++)
    {
        std::cout << "Bone " << i << " has " << boneCount[i] << " vertices" << std::endl;
    }

    // Allocate an array for all the indices (which is 3x the number of triangles)
    // Element array is an 1D array of integers
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; triIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pIndices[elementIndex + 0] = pTheTriangles[triIndex].v0;
        drawInfo.pIndices[elementIndex + 1] = pTheTriangles[triIndex].v1;
        drawInfo.pIndices[elementIndex + 2] = pTheTriangles[triIndex].v2;

        elementIndex += 3;      // Next "triangle"
    }


    return true;
}

bool cVAOManager::m_LoadTheFile_Ply_XYZ_N_RGBA_UV(std::string theFileName, sModelDrawInfo& drawInfo)
{
//    ply
//    format ascii 1.0
//    element vertex 66049
//    property float x
//    property float y
//    property float z
//    property float nx
//    property float ny
//    property float nz
//    property uchar red
//    property uchar green
//    property uchar blue
//    property uchar alpha
//    property float texture_u          <--- NEW!
//    property float texture_v          <--- NEW!

    //sVertexPlyFile p;       p.x = 0.0f;     p.y = 1.0f; p.z = 2.0f;
    //std::cout << p.x;

    //sVertexPlyFile q;
    //std::cout << "Type in the x: ";
    //std::cin >> q.x;

    // Input Filestream 
//    std::ifstream theBunnyFile("bathtub.ply");
    std::ifstream theBunnyFile( theFileName.c_str() );
    if (!theBunnyFile.is_open())
    {
        // didn't open the file.
        return false;
    }

    std::string temp;
    while (theBunnyFile >> temp)
    {
        if (temp == "vertex")
        {
            break;
        }
    };
    //element vertex 8171
//    theBunnyFile >> g_numberOfVertices;
    theBunnyFile >> drawInfo.numberOfVertices;


    while (theBunnyFile >> temp)
    {
        if (temp == "face")
        {
            break;
        }
    };
    //element vertex 8171
//    theBunnyFile >> g_numberOfTriangles;
    theBunnyFile >> drawInfo.numberOfTriangles;

    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

    while (theBunnyFile >> temp)
    {
        if (temp == "end_header")
        {
            break;
        }
    };


    // Allocate enough space to hold the vertices
//    sVertex vertices[8171];                 // Stack

//    sVertex x;      // STACK based variable (on the stack)
//    sVertex* px;    // Pointer variable.

//    int y = 5;
//
//    int* py = new int();
//    *py = 5;



    // This most closely matches the ply file for the bunny
    struct sVertexPlyFile
    {
        float x, y, z;
        float nx, ny, nz;
        float r, g, b, a;
        float u, v;             // <--- NEW!!!
    };

    struct sTrianglePlyFile
    {
    //    sVertexPlyFile verts[3];
    //    unsigned int vertIndexes[3];
        // Vertices of the triangles
        unsigned int v0, v1, v2;
    };

    // Dynamically allocate memory on the heap;
//    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[g_numberOfVertices];
    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[drawInfo.numberOfVertices];

    // -0.036872 0.127727 0.00440925 
//    for (unsigned int index = 0; index != g_numberOfVertices; index++)
    for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
    {
        sVertexPlyFile tempVertex;
        theBunnyFile >> tempVertex.x;                //std::cin >> a.x;
        theBunnyFile >> tempVertex.y;                //std::cin >> a.y;
        theBunnyFile >> tempVertex.z;                //std::cin >> a.z;

        theBunnyFile >> tempVertex.nx;
        theBunnyFile >> tempVertex.ny;
        theBunnyFile >> tempVertex.nz;

        theBunnyFile >> tempVertex.r;       tempVertex.r /= 255.0f;
        theBunnyFile >> tempVertex.g;       tempVertex.g /= 255.0f;
        theBunnyFile >> tempVertex.b;       tempVertex.b /= 255.0f;
        theBunnyFile >> tempVertex.a;       tempVertex.a /= 255.0f;

        // ALSO load the UV coordinates
        theBunnyFile >> tempVertex.u;
        theBunnyFile >> tempVertex.v;



        pTheVerticesFile[index] = tempVertex;
    }


//    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[g_numberOfTriangles];
    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[drawInfo.numberOfTriangles];

    // 3 3495 3549 3548 
    for (unsigned int index = 0; index != drawInfo.numberOfTriangles; index++)
    {
        sTrianglePlyFile tempTriangle;

        unsigned int discard;
        theBunnyFile >> discard;            // 3
        theBunnyFile >> tempTriangle.v0;                //std::cin >> a.x;
        theBunnyFile >> tempTriangle.v1;                //std::cin >> a.y;
        theBunnyFile >> tempTriangle.v2;                //std::cin >> a.z;

        pTheTriangles[index] = tempTriangle;
    }

    // Before, we manually looked up the vertex indices and added them to the vertex buffer
    //g_NumberOfVerticesToDraw = g_numberOfTriangles * 3;
    //pVertices = new sVertex[g_NumberOfVerticesToDraw];

    //unsigned int vertIndex = 0;
    //for (unsigned int triIndex = 0; triIndex != g_numberOfTriangles; triIndex++)
    //{
    //    // 3 1582 1581 2063 
    //    pVertices[vertIndex + 0].x = pTheVerticesFile[pTheTriangles[triIndex].v0].x;
    //    pVertices[vertIndex + 0].y = pTheVerticesFile[pTheTriangles[triIndex].v0].y;
    //    pVertices[vertIndex + 0].z = pTheVerticesFile[pTheTriangles[triIndex].v0].z;

    //    pVertices[vertIndex + 0].r = pTheVerticesFile[pTheTriangles[triIndex].v0].r;
    //    pVertices[vertIndex + 0].g = pTheVerticesFile[pTheTriangles[triIndex].v0].g;
    //    pVertices[vertIndex + 0].b = pTheVerticesFile[pTheTriangles[triIndex].v0].b;

    //    pVertices[vertIndex + 1].x = pTheVerticesFile[pTheTriangles[triIndex].v1].x;
    //    pVertices[vertIndex + 1].y = pTheVerticesFile[pTheTriangles[triIndex].v1].y;
    //    pVertices[vertIndex + 1].z = pTheVerticesFile[pTheTriangles[triIndex].v1].z;

    //    pVertices[vertIndex + 1].r = pTheVerticesFile[pTheTriangles[triIndex].v1].r;
    //    pVertices[vertIndex + 1].g = pTheVerticesFile[pTheTriangles[triIndex].v1].g;
    //    pVertices[vertIndex + 1].b = pTheVerticesFile[pTheTriangles[triIndex].v1].b;

    //    pVertices[vertIndex + 2].x = pTheVerticesFile[pTheTriangles[triIndex].v2].x;
    //    pVertices[vertIndex + 2].y = pTheVerticesFile[pTheTriangles[triIndex].v2].y;
    //    pVertices[vertIndex + 2].z = pTheVerticesFile[pTheTriangles[triIndex].v2].z;

    //    pVertices[vertIndex + 2].r = pTheVerticesFile[pTheTriangles[triIndex].v2].r;
    //    pVertices[vertIndex + 2].g = pTheVerticesFile[pTheTriangles[triIndex].v2].g;
    //    pVertices[vertIndex + 2].b = pTheVerticesFile[pTheTriangles[triIndex].v2].b;

    //    vertIndex += 3;
    //}

    // ... now we just copy the vertices from the file as is (unchanged)
    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pVertices[vertIndex].x = pTheVerticesFile[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTheVerticesFile[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTheVerticesFile[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = 1.0f;


        drawInfo.pVertices[vertIndex].nx = pTheVerticesFile[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTheVerticesFile[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTheVerticesFile[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = 1.0f;

        drawInfo.pVertices[vertIndex].r = pTheVerticesFile[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTheVerticesFile[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTheVerticesFile[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTheVerticesFile[vertIndex].a;

        // Copy the UV coords to the array that will eventually be copied to the GPU
        drawInfo.pVertices[vertIndex].u = pTheVerticesFile[vertIndex].u;
        drawInfo.pVertices[vertIndex].v = pTheVerticesFile[vertIndex].v;

    }

    // Allocate an array for all the indices (which is 3x the number of triangles)
    // Element array is an 1D array of integers
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; triIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pIndices[elementIndex + 0] = pTheTriangles[triIndex].v0;
        drawInfo.pIndices[elementIndex + 1] = pTheTriangles[triIndex].v1;
        drawInfo.pIndices[elementIndex + 2] = pTheTriangles[triIndex].v2;

        sVertex vertexOne = drawInfo.pVertices[drawInfo.pIndices[elementIndex + 0]];
        sVertex vertexTwo = drawInfo.pVertices[drawInfo.pIndices[elementIndex + 1]];
        sVertex vertexThree = drawInfo.pVertices[drawInfo.pIndices[elementIndex + 2]];

        glm::vec3 edgeOne = glm::vec3(vertexTwo.x, vertexTwo.y, vertexTwo.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);
        glm::vec3 edgeTwo = glm::vec3(vertexThree.x, vertexThree.y, vertexThree.z) - glm::vec3(vertexOne.x, vertexOne.y, vertexOne.z);

        glm::vec2 deltaUVOne = glm::vec2(vertexTwo.u, vertexTwo.v) - glm::vec2(vertexOne.u, vertexOne.v);
        glm::vec2 deltaUVTwo = glm::vec2(vertexThree.u, vertexThree.v) - glm::vec2(vertexOne.u, vertexOne.v);

        float fractional = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVTwo.x * deltaUVOne.y);

        glm::vec3 tangent;
        glm::vec3 bitangent;

        tangent.x = fractional * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
        tangent.y = fractional * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
        tangent.z = fractional * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);

        bitangent.x = fractional * (-deltaUVTwo.x * edgeOne.x + deltaUVOne.x * edgeTwo.x);
        bitangent.y = fractional * (-deltaUVTwo.x * edgeOne.y + deltaUVOne.x * edgeTwo.y);
        bitangent.z = fractional * (-deltaUVTwo.x * edgeOne.z + deltaUVOne.x * edgeTwo.z);

        tangent = glm::normalize(tangent);
        bitangent = glm::normalize(bitangent);

        
        for (int i = 0; i < 3; i++)
        {
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].tx = tangent.x;
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].ty = tangent.y;
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].tz = tangent.z;
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].tw = 1.0f;

            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].bx = bitangent.x;
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].by = bitangent.y;
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].bz = bitangent.z;
            drawInfo.pVertices[drawInfo.pIndices[elementIndex + i]].bw = 1.0f;
        }

        elementIndex += 3;      // Next "triangle"
    }


    return true;
}


    // Same as above but ONLY updates the vertex buffer information
bool cVAOManager::UpdateVertexBuffers(std::string fileName,
                         sModelDrawInfo& updatedDrawInfo,
                         unsigned int shaderProgramID)
{
        // This exists? 
    sModelDrawInfo updatedDrawInfo_TEMP;
    if (!this->FindDrawInfoByModelName(fileName, updatedDrawInfo_TEMP))
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VertexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
//                 (GLvoid*)updatedDrawInfo.pVertices,					
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,  // Offset
                    sizeof(sVertex) * updatedDrawInfo.numberOfVertices,
                    (GLvoid*)updatedDrawInfo.pVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}



bool cVAOManager::UpdateVAOBuffers(std::string fileName,
                      sModelDrawInfo& updatedDrawInfo,
                      unsigned int shaderProgramID)
{
    // This exists? 
    sModelDrawInfo updatedDrawInfo_TEMP;
    if ( ! this->FindDrawInfoByModelName(fileName,  updatedDrawInfo_TEMP) )
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VertexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
//                 (GLvoid*)updatedDrawInfo.pVertices,					
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 
                    0,  // Offset
                    sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
                    (GLvoid*)updatedDrawInfo.pVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, updatedDrawInfo.IndexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
//                 sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
//                 (GLvoid*)updatedDrawInfo.pIndices,
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,			
                    0,  // Offset
                    sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
                    (GLvoid*)updatedDrawInfo.pIndices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    return true;
}
