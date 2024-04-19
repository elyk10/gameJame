#include "cPlayer.h"

cPlayer::cPlayer()
{

}

cPlayer::cPlayer(cMesh* thePlayer)
{
	this->theMesh = thePlayer;
	animationPlaying = 0;
	prevAnimation = animationPlaying;
	lookingAt = glm::vec3(0.0f, 0.0f, -1.0f);
	jumping = false;
	attacking = false;
}

cPlayer::~cPlayer()
{

}