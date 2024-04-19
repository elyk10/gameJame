#pragma once

#include "cMesh.h"
#include "cPlayer.h"

enum class  eEnemyBehaviour
{
	None,
	Seek,
	Flee,
	Pursue,
	Evade,
	Approach, 
	Wander, 
	PathFollowing
};

struct sWanderData
{
	glm::vec3 targetDirection;
	float radiusOne;
	float radiusTwo;
	float decisionTime;
	float decisionCountdown;
};

struct sPathData
{
	float innerRadius;
	float outterRadius;
	float pathRadius;
	std::vector<glm::vec3> pointsOnPath;
};

class cEnemy
{
public:
	cEnemy();
	cEnemy(cMesh* enemyMesh);
	cEnemy(cMesh* enemyMesh, eEnemyBehaviour initBehaviour);

	~cEnemy();

	void Update(double deltaTime);
	void UpdateBehaviour(eEnemyBehaviour newBehaviour);

	 

	cMesh* theMesh;
	float speed;
	eEnemyBehaviour behaviour;
	cPlayer* target;
	glm::vec3 forward;
	int animationPlaying; 
	int prevAnimation;

	float approachRadius;
	sWanderData wanderData;
	sPathData pathData;

private:
	void Wander(double deltaTime);
	void Seek(double deltaTime);
	void Flee(double deltaTime);
	void Approach(double deltaTime);
	void Pursue(double deltaTime);
	void Evade(double deltaTime);
	void PathFollow(double deltaTime); 
	void Face(double deltaTime, glm::vec3& targetDirection);

	glm::vec3 ClosestPoint(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC, float& t);
	

};
