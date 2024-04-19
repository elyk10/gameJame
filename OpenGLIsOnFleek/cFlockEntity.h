#pragma once
#include "cMesh.h"

class cFlockEntity
{
public:
	struct sFlockEntityDef
	{
		std::vector<cMesh*> theMeshes;
		glm::vec3 minBoundary;
		glm::vec3 maxBoundary;
		float neighboutRadius;
		size_t flockCount;
		float dangerRadius;
		float separationFactor;
		float cohesionFactor;
		float alignmentFactor;
	};
	class cFlockMember
	{
	public:
		cFlockMember(cMesh* theParentMesh);
		~cFlockMember();

		void Flock(double deltaTime, float dangerRadiusSquared, float separationFactor, float cohesionFactor, float alignmentFactor);

		void Face(double deltaTime, float factor, const glm::vec3& targetDirection);

		cMesh* theMesh;
		glm::vec3 forward;

		std::vector<cFlockMember*> Neighbours; 
	};

public:
	cFlockEntity();
	cFlockEntity(sFlockEntityDef& def);

	~cFlockEntity();

	void Update(double deltaTime);
	void BuildNeighboutLists(cFlockMember* memberA, cFlockMember* memberB);

	std::vector<cMesh*> theMeshes; 
	glm::vec3 mMinBoundary;
	glm::vec3 mMaxBoundary;
	float mNeighboutRadiusSquared;
	std::vector<cFlockMember*> mFlockMembers;
	float mDangerRadiusSquared;
	float mSeparationFactor;
	float mCohesionFactor;
	float mAlignmentFactor;

};

