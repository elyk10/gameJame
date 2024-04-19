#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <map>

#include <assimp/scene.h>

#include "../Animation.h"

// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVertex
{
	float x, y, z, w;		// vPos
	float r, g, b, a;		// vCol
	float nx, ny, nz, nw;	// vNormal
	float u, v, u2, v2;				// vTextureCoords		<--- NEW!!
	float tx, ty, tz, tw;			// tangent
	float bx, by, bz, bw;			// bitangent
	float bone1, bone2, bone3, bone4; // vBoneIndex
	float bWeight1, bWeight2, bWeight3, bWeight4; // vBoneWeight
};

struct BoneInfo
{
	glm::mat4 BoneOffset;				// Offset from the parent bone/node
	glm::mat4 FinalTransformation;		// Calculated transformation used for rendering
	glm::mat4 GlobalTransformation;		// used for the bone hierarchy transformation calculations when animating
};

struct Node
{
	Node(const std::string& name) : Name(name) {}
	std::string Name;
	glm::mat4 transformation;
	std::vector<Node*> children;
};

struct NodeAnim
{
	NodeAnim(const std::string& name) : Name(name) {}
	std::string Name;
	std::vector<PositionKeyFrame> m_PositionKeyFrames;
	std::vector<ScaleKeyFrame> m_ScaleKeyFrames;
	std::vector<RotationKeyFrame> m_RotationKeyFrames;
};

struct CharacterAnimation
{
	std::string Name;
	double ticksPerSecond;
	double duration;
	double currentTime;
	Node* RootNode;
	std::vector<NodeAnim*> Channels;

	NodeAnim* findNodeAnim(const std::string& name)
	{
		for (int i = 0; i < Channels.size(); i++)
		{
			if (Channels[i]->Name == name)
			{
				return Channels[i];
			}
		}
		return nullptr;
		
		/*auto iter = std::find_if(Channels.begin(), Channels.end(),
			[&](const NodeAnim& Bone)
			{
				return Bone.Name == name;
			}
		);
		if (iter == Channels.end()) return nullptr;
		else return (*iter);*/
	}
};



struct sModelDrawInfo
{
	sModelDrawInfo();
	~sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;	// Triangles

	glm::vec3 maxExtents_XYZ;	// bounding box maximums
	glm::vec3 minExtents_XYZ;	// bounding box minimums
	glm::vec3 deltaExtents_XYZ;	// bounding box dimensions
	float maxExtent;

	std::vector<glm::mat4> NodeHeirarchyTransformations;
	std::map<std::string, int> NodeNameToIdMap;

	std::vector<BoneInfo> BoneInfoVec;
	std::map<std::string, int> BoneNameToIdMap; 
	Node* RootNode;

	std::vector<CharacterAnimation*> CharacterAnimations; 

	CharacterAnimation* BlendAnimation;
	bool hasBlend;
	/*
	have to store the finalPosition, finalRotation, and finalScale some how when the blending is to happen
	then have to frames. the one with the final position and the one at the start of the next animation
	can create a new character animation with those two and update it over a certain amount of time (say about a 
	second?) then it would have to set the playing animation to the next animation it is blending to
	probably best to put this in a method of some sorts
	*/


	glm::mat4 GlobalInverseTransformation;

	void calcExtents(void);

	void FreeMemory(void);

	Node* GenerateBoneHierarchy(aiNode* assimpNode, const int depth = 0);

	void AssimpToGLM(const aiMatrix4x4& a, glm::mat4& g);

	Node* CreateAnimNode(aiNode* node);

	int FindBoneInfoByName(std::string boneName);

	// 
	unsigned int getUniqueID(void);
private:
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1;
	static unsigned int m_nextUniqueID;
};
