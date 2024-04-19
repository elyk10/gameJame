#include "cFlockEntity.h"
#include <glm/gtx/norm.hpp> 
#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp> 
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>


cFlockEntity::cFlockEntity()
{

}
cFlockEntity::cFlockEntity(sFlockEntityDef& def)
	: mMinBoundary(def.minBoundary),
	mMaxBoundary(def.maxBoundary),
	mNeighboutRadiusSquared(def.neighboutRadius * def.neighboutRadius), 
	theMeshes(def.theMeshes), 
	mDangerRadiusSquared(def.dangerRadius * def.dangerRadius), 
	mSeparationFactor(def.separationFactor), 
	mCohesionFactor(def.cohesionFactor), 
	mAlignmentFactor(def.alignmentFactor)
{
	mFlockMembers.resize(def.flockCount);
	
	for (unsigned int i = 0; i < def.flockCount; i++)
	{
		mFlockMembers[i] = new cFlockMember(theMeshes[i]);
		glm::vec3 position(glm::linearRand(mMinBoundary, mMaxBoundary));
		//position.y = 10.0f;
		mFlockMembers[i]->theMesh->drawPosition = position;

	}
}

cFlockEntity::~cFlockEntity()
{
	for (unsigned int i = 0; i < mFlockMembers.size(); i++)
	{
		delete mFlockMembers[i];
	}
	mFlockMembers.clear();
}

void cFlockEntity::Update(double deltaTime)
{
	// clear neighbour list
	for (unsigned int i = 0; i < mFlockMembers.size(); i++)
	{
		mFlockMembers[i]->Neighbours.clear();
	}
	
	// update neighbour list
	for (unsigned int i = 0; i < mFlockMembers.size() - 1; i++)
	{
		for (unsigned int j = i + 1; j < mFlockMembers.size(); j++)
		{
			BuildNeighboutLists(mFlockMembers[i], mFlockMembers[j]);
		}
	}

	//tell them to flock
	for (unsigned int i = 0; i < mFlockMembers.size(); i++)
	{
		mFlockMembers[i]->Flock(deltaTime, mDangerRadiusSquared, mSeparationFactor, mCohesionFactor, mAlignmentFactor);
		//std::cout << "Member[" << i << "]: " << mFlockMembers[i]->forward.x <<
		//	", " << mFlockMembers[i]->forward.y << ", " << mFlockMembers[i]->forward.z << std::endl;
		//mFlockMembers[i]->theMesh->drawPosition -= mFlockMembers[i]->forward * (float)deltaTime;
	}
}

void cFlockEntity::BuildNeighboutLists(cFlockMember* memberA, cFlockMember* memberB)
{
	glm::vec3 posA(memberA->theMesh->drawPosition);
	glm::vec3 posB(memberB->theMesh->drawPosition);

	if (glm::length2(posB - posA) < mNeighboutRadiusSquared)
	{
		memberA->Neighbours.push_back(memberB);
		memberB->Neighbours.push_back(memberA);
	}
}

cFlockEntity::cFlockMember::cFlockMember(cMesh* theParentMesh)
{
	theMesh = theParentMesh;
	forward = glm::vec3(0.0f, 0.0f, 1.0f);
}

cFlockEntity::cFlockMember::~cFlockMember()
{
	delete theMesh;
}

void cFlockEntity::cFlockMember::Flock(double deltaTime, float dangerRadiusSquared, float separationFactor, float cohesionFactor, float alignmentFactor)
{
	if (Neighbours.size() == 0)
	{
		return;
	}
	// calculate separation
	// // look at all the neighbours and calculate an average position. turn away from that average position
	// calculate cohesion
	// // look at all the neighbours and calculate an average position. turn toward that average position
	// calculate alignment
	// // look at all the neighbours and calculate an average forward direction. turn toward that forward direction
	glm::vec3 alignmentDirection(0.0f);
	unsigned int numNeighbours = Neighbours.size();
	for (cFlockMember* other : Neighbours)
	{
		alignmentDirection += other->forward;
	}
	alignmentDirection /= numNeighbours;
	Face(deltaTime, alignmentFactor, alignmentDirection);

	glm::vec3 cohesionPosition(0.0f);
	for (cFlockMember* other : Neighbours)
	{
		cohesionPosition += other->theMesh->drawPosition;
	}
	cohesionPosition /= numNeighbours;
	Face(deltaTime, cohesionFactor, cohesionPosition - theMesh->drawPosition);

	numNeighbours = 0;
	glm::vec3 separationPosition(0.0f);
	for (cFlockMember* other : Neighbours)
	{
		if (glm::length2(other->theMesh->drawPosition - theMesh->drawPosition) < dangerRadiusSquared)
		{
			numNeighbours++;
			separationPosition += other->theMesh->drawPosition;
		}
	}
	if (numNeighbours > 0)
	{
		separationPosition /= numNeighbours;
		Face(deltaTime, separationFactor, theMesh->drawPosition - separationPosition);
	}

	theMesh->drawPosition -= forward * (float)deltaTime;  
}

void cFlockEntity::cFlockMember::Face(double deltaTime, float factor, const glm::vec3& targetDirection)
{
	float rotationSpeed = 1.0f;
	float rotationStep = rotationSpeed * (float)deltaTime;
	//glm::vec3 targetPos = this->target->theMesh->drawPosition;

	glm::vec3 goalDirection = targetDirection;
	
	float angle = glm::angle(goalDirection, this->forward);

	if (angle < rotationStep)
	{
		return;
	}
	glm::vec3 fixVec = glm::vec3(0.0f); // will fix the orientation
	glm::vec3 rotationVec = glm::cross(this->forward, goalDirection);
	if (rotationVec.y > 0.0f)
	{
		rotationVec = glm::vec3(0.0f, 1.0f * factor, 0.0f);
	}
	else
	{
		rotationVec = glm::vec3(0.0f, -1.0f * factor, 0.0f);
		fixVec = glm::vec3(0.0f, 0.0f, -3.14159f);
	}
	//rotationVec = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 temp = glm::cross(rotationVec, this->forward);

	glm::vec3 newForward = glm::rotate(this->forward, rotationStep, rotationVec);//cos(rotationStep) * this->forward + sin(rotationStep) * temp;


	this->forward = newForward;
	this->theMesh->setDrawOrientation(glm::quatLookAt(newForward, rotationVec));
	this->theMesh->adjustRoationAngleFromEuler(fixVec);
}

