#pragma once

#include "cMesh.h"
#include "cVAOManager/sModelDrawInfo.h"
#include <glm/gtx/easing.hpp>
#include <iostream>

class AnimationManager
{
public:
	void Update(cMesh* meshToUpdate);
	void UpdateAll(const std::vector<cMesh*>& meshesToUpdate);
	
	void UpdateCharacter(const cMesh* characterUpdate, int animation, sModelDrawInfo* model, double deltaTime, glm::vec3 lookingAt = glm::vec3(0.0f));

	void BlendAnimations(cMesh* characterUpdate, int animationOne, int animationTwo, sModelDrawInfo* model, double deltaTime, glm::vec3 lookingAt = glm::vec3(0.0f));

private:
	void CalculateMatrices(sModelDrawInfo* model, CharacterAnimation* animation, Node* node, const glm::mat4& parentTransformation, double keyFrameTime, glm::vec3 lookingAt);
	glm::mat4 NodeAnimMatrix(NodeAnim* node, double keyFrameTime);

	

};
