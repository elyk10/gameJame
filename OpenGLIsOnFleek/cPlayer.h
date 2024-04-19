#pragma once

#include "cMesh.h"
#include "sPhsyicsProperties.h"

class cPlayer
{
public:
	cPlayer();
	cPlayer(cMesh* playerMesh);

	~cPlayer();

	cMesh* theMesh;
	sPhsyicsProperties* thePhysics;
	float speed;
	glm::vec3 lookingAt;
	glm::vec3 bodyDir;
	glm::vec3 moveDir;
	int animationPlaying;
	int prevAnimation;
	bool sprint;
	bool jumping;
	bool attacking;
};
