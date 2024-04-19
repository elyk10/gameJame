#include "sModelDrawInfo.h"

void sModelDrawInfo::AssimpToGLM(const aiMatrix4x4& a, glm::mat4& g)
{
	g[0][0] = a.a1; g[0][1] = a.b1; g[0][2] = a.c1; g[0][3] = a.d1;
	g[1][0] = a.a2; g[1][1] = a.b2; g[1][2] = a.c2; g[1][3] = a.d2;
	g[2][0] = a.a3; g[2][1] = a.b3; g[2][2] = a.c3; g[2][3] = a.d3;
	g[3][0] = a.a4; g[3][1] = a.b4; g[3][2] = a.c4; g[3][3] = a.d4;
}

Node* sModelDrawInfo::CreateAnimNode(aiNode* node) 
{
	Node* newNode = new Node(node->mName.C_Str());
	AssimpToGLM(node->mTransformation, newNode->transformation);
	return newNode;
}


sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	this->maxExtents_XYZ = glm::vec3(0.0f);
	this->minExtents_XYZ = glm::vec3(0.0f);
	this->deltaExtents_XYZ = glm::vec3(0.0f);

//	scale = 5.0/maxExtent;		-> 5x5x5
	this->maxExtent = 0.0f;


	this->m_UniqueID = sModelDrawInfo::m_nextUniqueID;
	sModelDrawInfo::m_nextUniqueID++;

	hasBlend = false;

	return;
}

sModelDrawInfo::~sModelDrawInfo()
{

}

void sModelDrawInfo::FreeMemory(void)
{
	if (this->pVertices)
	{
		delete[] this->pVertices;
	}
	if (this->pIndices)
	{
		delete[] this->pIndices;
	}
	return;
}

//static 
unsigned int sModelDrawInfo::m_nextUniqueID = sModelDrawInfo::FIRST_UNIQUE_ID;


unsigned int sModelDrawInfo::getUniqueID(void)
{
	return this->m_UniqueID;
}

void sModelDrawInfo::calcExtents(void)
{
	if ( this->pVertices == NULL )
	{
		// No vertices to check
		return;
	}

	// Set the 1st vertex as max and min
	sVertex* pCurrentVert = &(this->pVertices[0]);
	
	this->minExtents_XYZ.x = pCurrentVert->x;
	this->minExtents_XYZ.y = pCurrentVert->y;
	this->minExtents_XYZ.z = pCurrentVert->z;

	this->maxExtents_XYZ.x = pCurrentVert->x;
	this->maxExtents_XYZ.y = pCurrentVert->y;
	this->maxExtents_XYZ.z = pCurrentVert->z;

	for ( unsigned int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++ )
	{
		sVertex* pCurrentVert = &(this->pVertices[vertIndex]);

		if (pCurrentVert->x < this->minExtents_XYZ.x) { this->minExtents_XYZ.x = pCurrentVert->x; }
		if (pCurrentVert->y < this->minExtents_XYZ.y) { this->minExtents_XYZ.y = pCurrentVert->y; }
		if (pCurrentVert->z < this->minExtents_XYZ.z) { this->minExtents_XYZ.z = pCurrentVert->z; }

		if (pCurrentVert->x > this->maxExtents_XYZ.x) { this->maxExtents_XYZ.x = pCurrentVert->x; }
		if (pCurrentVert->y > this->maxExtents_XYZ.y) { this->maxExtents_XYZ.y = pCurrentVert->y; }
		if (pCurrentVert->z > this->maxExtents_XYZ.z) { this->maxExtents_XYZ.z = pCurrentVert->z; }

	}//for ( unsigned int vertIndex = 0...

	// Calculate deltas...
	this->deltaExtents_XYZ.x = this->maxExtents_XYZ.x - this->minExtents_XYZ.x;
	this->deltaExtents_XYZ.y = this->maxExtents_XYZ.y - this->minExtents_XYZ.y;
	this->deltaExtents_XYZ.z = this->maxExtents_XYZ.z - this->minExtents_XYZ.z;

	// Largest extent...
	this->maxExtent = this->deltaExtents_XYZ.x;
	if (this->maxExtent < this->deltaExtents_XYZ.y) { this->maxExtent = this->deltaExtents_XYZ.y; }
	if (this->maxExtent < this->deltaExtents_XYZ.z) { this->maxExtent = this->deltaExtents_XYZ.z; }

	return;
}

Node* sModelDrawInfo::GenerateBoneHierarchy(aiNode* assimpNode, const int depth)
{
	Node* node = CreateAnimNode(assimpNode);
	/*PRINT_SPACES(depth);*/ printf("%s\n", assimpNode->mName.C_Str());
	aiMatrix4x4& transformation = assimpNode->mTransformation;
	aiVector3D position;
	aiQuaternion rotation;
	aiVector3D scaling;
	transformation.Decompose(scaling, rotation, position);
	///*PRINT_SPACES(depth + 1);*/ printf("Position: {%.3f, %.3f, %.3f}\n", position.x, position.y, position.z);
	///*PRINT_SPACES(depth + 1); */ printf("Scaling: {%.3f, %.3f, %.3f}\n", scaling.x, scaling.y, scaling.z);
	///*PRINT_SPACES(depth + 1); */ printf("Quaternion: {%.3f, %.3f, %.3f, %.3f}\n", rotation.x, rotation.y, rotation.z, rotation.w);
	//printf("\n");

	glm::mat4 glmMatrix;
	AssimpToGLM(transformation, glmMatrix);

	//glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));
	//glm::mat4 RotationMatrix = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
	//glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));

	//glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;

	NodeNameToIdMap.insert(std::pair<std::string, int>(assimpNode->mName.C_Str(), NodeHeirarchyTransformations.size()));
	NodeHeirarchyTransformations.emplace_back(glmMatrix);

	for (int i = 0; i < assimpNode->mNumChildren; ++i)
	{
		node->children.emplace_back(GenerateBoneHierarchy(assimpNode->mChildren[i], depth + 1)); 
	}
	return node;
}

int sModelDrawInfo::FindBoneInfoByName(std::string boneName)
{
	std::map<std::string, int>::iterator itBoneInfo = this->BoneNameToIdMap.find(boneName);

	if (itBoneInfo == this->BoneNameToIdMap.end())
	{
		return -1;
	}

	return itBoneInfo->second;
}
