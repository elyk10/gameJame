#include "cEnemy.h"
#include <glm/gtx/vector_angle.hpp> 
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtc/random.hpp>
#include <iostream>

cEnemy::cEnemy()
{
	//theMesh->bUseDebugColours = true; // to show behaviour
	animationPlaying = 0;
	prevAnimation = animationPlaying;

}

cEnemy::cEnemy(cMesh* enemyMesh)
{
	this->theMesh = enemyMesh;
	//theMesh->bUseDebugColours = true; // to show behaviour
	this->behaviour = eEnemyBehaviour::None;
	animationPlaying = 0;
	prevAnimation = animationPlaying;

}

cEnemy::cEnemy(cMesh* enemyMesh, eEnemyBehaviour initBehaviour)
{
	this->theMesh = enemyMesh;
	//theMesh->bUseDebugColours = true; // to show behaviour
	this->UpdateBehaviour(initBehaviour);
	animationPlaying = 0;
	prevAnimation = animationPlaying;


	approachRadius = 10.0f;

	if (initBehaviour == eEnemyBehaviour::Wander)
	{
		this->wanderData.radiusOne = 5.0f;
		this->wanderData.radiusTwo = 2.0f;
		this->wanderData.decisionTime = 2.0f;
		this->wanderData.decisionCountdown = 0.0f;
		this->wanderData.targetDirection = glm::vec3(0.0f);
	}

	if (initBehaviour == eEnemyBehaviour::PathFollowing)
	{
		this->pathData.innerRadius = 30.0f;
		this->pathData.outterRadius = 40.0f;
		this->pathData.pathRadius = 2.5f;
		std::vector<glm::vec3> path;
		path.push_back(glm::vec3(50.0f, 0.0f, 50.0f));
		path.push_back(glm::vec3(-50.0f, 0.0f, 50.0f));
		path.push_back(glm::vec3(-50.0f, 0.0f, -50.0f));
		path.push_back(glm::vec3(50.0f, 0.0f, -50.0f));
		
		this->pathData.pointsOnPath = path; 
	}
}

cEnemy::~cEnemy()
{

}

void cEnemy::Face(double deltaTime, glm::vec3& targetDirection)
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
		rotationVec = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		rotationVec = glm::vec3(0.0f, -1.0f, 0.0f);
		fixVec = glm::vec3(0.0f, 0.0f, -3.14159f);
	}
	//rotationVec = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 temp = glm::cross(rotationVec, this->forward);

	glm::vec3 newForward = glm::rotate(forward, rotationStep, rotationVec);//cos(rotationStep) * this->forward + sin(rotationStep) * temp;


	this->forward = newForward;
	this->theMesh->setDrawOrientation(glm::quatLookAt(newForward, rotationVec));
	this->theMesh->adjustRoationAngleFromEuler(fixVec);

}

void cEnemy::Update(double deltaTime)
{
	switch (this->behaviour)
	{
	case (eEnemyBehaviour::None):
		return;
		break;
	case (eEnemyBehaviour::Seek): // red 
		this->Seek(deltaTime);
		break; 
	case (eEnemyBehaviour::Flee): // blue
		this->Flee(deltaTime);
		break;
	case (eEnemyBehaviour::Pursue): // green 
		this->Pursue(deltaTime);
		break;
	case (eEnemyBehaviour::Evade): // yellow 
		this->Evade(deltaTime);
		break;
	case (eEnemyBehaviour::Approach): // purple 
		this->Approach(deltaTime);
		break;
	case (eEnemyBehaviour::Wander):
		this->Wander(deltaTime);
		break;
	//case (eEnemyBehaviour::PathFollowing):
		//this->PathFollow(deltaTime);
		break;
	}

	if (behaviour == eEnemyBehaviour::Approach)
	{
		if (glm::distance(this->theMesh->drawPosition, this->target->theMesh->drawPosition) > approachRadius)
		{
			this->theMesh->drawPosition -= this->forward * this->speed * (float)deltaTime;
		}
	}
	if (behaviour == eEnemyBehaviour::PathFollowing)
	{
		if (glm::distance(this->theMesh->drawPosition, this->target->theMesh->drawPosition) < pathData.outterRadius)
		{
			this->Seek(deltaTime);
			this->theMesh->drawPosition -= this->forward * this->speed * (float)deltaTime;
			if (glm::distance(this->theMesh->drawPosition, this->target->theMesh->drawPosition) < pathData.innerRadius)
			{
				//this->theMesh->drawPosition = glm::vec3(-40.0f, this->theMesh->drawPosition.y, 53.0f);
			}
		}
		else
		{
			this->PathFollow(deltaTime);
			this->theMesh->drawPosition -= this->forward * this->speed * (float)deltaTime;
		}
	}
	else
	{
		this->theMesh->drawPosition -= this->forward * this->speed * (float)deltaTime;
	}

	return;
}

void cEnemy::UpdateBehaviour(eEnemyBehaviour newBehaviour)
{
	//switch (newBehaviour)
	//{
	//case (eEnemyBehaviour::None):
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//	break;
	//case (eEnemyBehaviour::Seek): // red
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//	break;
	//case (eEnemyBehaviour::Flee): // blue
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	//	break;
	//case (eEnemyBehaviour::Pursue): // green
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//	break;
	//case (eEnemyBehaviour::Evade): // yellow
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	//	break;
	//case (eEnemyBehaviour::Approach): // purple
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	//	break;
	//case(eEnemyBehaviour::PathFollowing):
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	//	break;
	//case(eEnemyBehaviour::Wander): // orange
	//	float colourAmount = this->wanderData.radiusOne;
	//	colourAmount /= 2.0f;
	//	while (colourAmount > 0.5f)
	//	{
	//		colourAmount /= 10.0f;
	//	}
	//	theMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.25f + colourAmount, 0.0f, 1.0f);
	//	break;
	//
	//}

	this->behaviour = newBehaviour;
}

void cEnemy::Wander(double deltaTime)
{
	this->wanderData.decisionCountdown -= deltaTime;
	
	if (this->wanderData.decisionCountdown < 0.0f)
	{
		this->wanderData.decisionCountdown = this->wanderData.decisionTime;

		glm::vec3 direction = this->forward;

		glm::vec3 wanderPoint(this->theMesh->drawPosition + direction * this->wanderData.radiusOne);

		float theta = glm::linearRand(0.0f, glm::two_pi<float>());

		float x = this->wanderData.radiusTwo * glm::cos(theta);
		float z = this->wanderData.radiusTwo * glm::sin(theta);

		wanderPoint.x += x;
		wanderPoint.z += z;

		this->wanderData.targetDirection = glm::normalize(wanderPoint - this->theMesh->drawPosition);
	}

	this->Face(deltaTime, this->wanderData.targetDirection);
}

void cEnemy::Seek(double deltaTime)
{
	glm::vec3 targetPos = this->target->theMesh->drawPosition;
	glm::vec3 goalDirection = glm::normalize(this->theMesh->drawPosition - glm::vec3(targetPos.x, 0.0f, targetPos.z)); 
	this->Face(deltaTime, goalDirection); 
}
void cEnemy::Flee(double deltaTime)
{
	glm::vec3 targetPos = this->target->theMesh->drawPosition;
	glm::vec3 goalDirection = glm::normalize(glm::vec3(targetPos.x, 0.0f, targetPos.z) - this->theMesh->drawPosition);
	this->Face(deltaTime, goalDirection);
}
void cEnemy::Approach(double deltaTime)
{
	glm::vec3 targetPos = this->target->theMesh->drawPosition;
	glm::vec3 goalDirection = glm::normalize(this->theMesh->drawPosition - glm::vec3(targetPos.x, 0.0f, targetPos.z));
	this->Face(deltaTime, goalDirection);
}
void cEnemy::Pursue(double deltaTime)
{
	glm::vec3 targetPos = this->target->theMesh->drawPosition;
	glm::vec3 temp = targetPos + this->target->moveDir * 4.0f;
	glm::vec3 goalDirection = glm::normalize(this->theMesh->drawPosition - glm::vec3(temp.x, 0.0f, temp.z));
	this->Face(deltaTime, goalDirection);
}
void cEnemy::Evade(double deltaTime)
{
	glm::vec3 targetPos = this->target->theMesh->drawPosition;
	glm::vec3 temp = targetPos + this->target->moveDir * 4.0f;
	glm::vec3 goalDirection = glm::normalize(glm::vec3(temp.x, 0.0f, temp.z) - this->theMesh->drawPosition);
	this->Face(deltaTime, goalDirection);
}

void cEnemy::PathFollow(double deltaTime)
{
	glm::vec3 futurePos = theMesh->drawPosition + forward * 5.0f;

	float shortestDistance = std::numeric_limits<float>::max();
	glm::vec3 targetPos;

	for (int i = 0; i < pathData.pointsOnPath.size(); i++)
	{
		glm::vec3 pointA = pathData.pointsOnPath[i];
		glm::vec3 pointB;
		if (i == pathData.pointsOnPath.size() - 1)
		{
			pointB = pathData.pointsOnPath[0];
		}
		else
		{
			pointB = pathData.pointsOnPath[i + 1];
		}

		float t(0.0f);
		glm::vec3 closestPoint = this->ClosestPoint(pointA, pointB, futurePos, t);

		float distance = glm::distance(futurePos, closestPoint);
		if (distance < shortestDistance)
		{
			shortestDistance = distance;

			targetPos = closestPoint;

			targetPos += glm::normalize(pointB - pointA) * shortestDistance;
		}
	}

	if (shortestDistance > pathData.pathRadius)
	{
		glm::vec3 goalDirection = glm::normalize(this->theMesh->drawPosition - glm::vec3(targetPos.x, 0.0, targetPos.z));
		this->Face(deltaTime, goalDirection);
	}

}


glm::vec3 cEnemy::ClosestPoint(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC, float& t)
{
	// project c onto ab
	t = glm::dot(pointC - pointA, pointB - pointA) / glm::dot(pointB - pointA, pointB - pointA);

	//clamp t to a 0-1 range. if t > 1 or t < 0 then the closest point is outside the line
	t = glm::clamp(t, 0.0f, 1.0f);

	// compute the projected position from the clamped t value
	return glm::vec3(pointA + t * (pointB - pointA));
}