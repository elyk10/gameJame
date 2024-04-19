#include "AnimationManager.h"
#include <glm/gtx/quaternion.hpp>

void AnimationManager::Update(cMesh* meshToUpdate)
{
	cMesh* gameObject = meshToUpdate;

	if (meshToUpdate->m_Animation != nullptr)
	{
		Animation* animation = meshToUpdate->m_Animation;
		double time = animation->m_Time;

		// Position
		if (animation->m_PositionKeyFrames.size() != 0)
		{


			// If there is only 1 key frame, use that one.
			// If we are equal to or greater than the time of the last key frame, use the last keyframe
			if (animation->m_PositionKeyFrames.size() == 1)
			{
				gameObject->drawPosition = animation->m_PositionKeyFrames[0].m_Position;
			}
			else
			{
				// Find our position keyframes (Start and End)
				int KeyFrameEndIndex = 0;
				for (; KeyFrameEndIndex < animation->m_PositionKeyFrames.size(); KeyFrameEndIndex++)
				{
					//std::cout << "time: " << time << " m_time: " << animation->m_PositionKeyFrames[KeyFrameEndIndex].m_Time << std::endl;
					if (animation->m_PositionKeyFrames[KeyFrameEndIndex].m_Time > time)
					{
						break;
					}
				}

				if (KeyFrameEndIndex >= animation->m_PositionKeyFrames.size())
				{
					// we are at or past the last key frame use the last keyframe only
					gameObject->drawPosition = animation->m_PositionKeyFrames[KeyFrameEndIndex - 1].m_Position;
					return;
				}
				int KeyFrameStartIndex = KeyFrameEndIndex - 1;


				PositionKeyFrame startKeyFrame = animation->m_PositionKeyFrames[KeyFrameStartIndex];
				PositionKeyFrame endKeyFrame = animation->m_PositionKeyFrames[KeyFrameEndIndex];

				float percent = (time - startKeyFrame.m_Time) / (endKeyFrame.m_Time - startKeyFrame.m_Time);
				float result = 0.f;
				switch (endKeyFrame.n_EaseType)
				{
				case EasingType::Linear:
					result = percent;
					//gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f); // white
					break;

				case EasingType::sineEaseIn:
					result = glm::sineEaseIn(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
					break;

				case EasingType::sineEaseOut:
					result = glm::sineEaseOut(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
					break;

				case EasingType::sineEaseInOut:
					result = glm::sineEaseInOut(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					break;
				}

				glm::vec3 delta = endKeyFrame.m_Position - startKeyFrame.m_Position;
				//std::cout << delta.x << delta.y << delta.z << std::endl;
				gameObject->drawPosition = startKeyFrame.m_Position + delta * result;
			}
		}


		// Rotation
		if (animation->m_RotationKeyFrames.size() != 0)
		{
			if (animation->m_RotationKeyFrames.size() == 1)
			{
				gameObject->setDrawOrientation(animation->m_RotationKeyFrames[0].m_Rotation);
			}
			else
			{
				int keyFrameEndIndex = 0;
				for (; keyFrameEndIndex < animation->m_RotationKeyFrames.size(); keyFrameEndIndex++)
				{
					if (animation->m_RotationKeyFrames[keyFrameEndIndex].m_Time > time)
					{
						break;
					}
				}

				if (keyFrameEndIndex >= animation->m_RotationKeyFrames.size())
				{
					gameObject->setDrawOrientation(animation->m_RotationKeyFrames[keyFrameEndIndex - 1].m_Rotation);
					return;
				}

				int keyFrameStartIndex = keyFrameEndIndex - 1;

				RotationKeyFrame startKeyFrame = animation->m_RotationKeyFrames[keyFrameStartIndex];
				RotationKeyFrame endKeyFrame = animation->m_RotationKeyFrames[keyFrameEndIndex];

				float percent = (time - startKeyFrame.m_Time) / (endKeyFrame.m_Time - startKeyFrame.m_Time);
				float result = 0;

				switch (endKeyFrame.n_EaseType)
				{
				case EasingType::Linear:
					result = percent;
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f); // white
					break;

				case EasingType::sineEaseIn:
					result = glm::sineEaseIn(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
					break;

				case EasingType::sineEaseOut:
					result = glm::sineEaseOut(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
					break;

				case EasingType::sineEaseInOut:
					result = glm::sineEaseInOut(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					break;

				}

				gameObject->setDrawOrientation(glm::slerp(startKeyFrame.m_Rotation, endKeyFrame.m_Rotation, result));
			}
		}

		// Scale 
		if (animation->m_ScaleKeyFrames.size() != 0)
		{
			if (animation->m_ScaleKeyFrames.size() == 1)
			{
				gameObject->drawScale = animation->m_ScaleKeyFrames[0].m_Scale;
			}
			else
			{
				int keyFrameEndIndex = 0;
				for (; keyFrameEndIndex < animation->m_ScaleKeyFrames.size(); keyFrameEndIndex++)
				{
					if (animation->m_ScaleKeyFrames[keyFrameEndIndex].m_Time > time)
					{
						break;
					}
				}

				if (keyFrameEndIndex >= animation->m_ScaleKeyFrames.size())
				{
					gameObject->drawScale = animation->m_ScaleKeyFrames[keyFrameEndIndex - 1].m_Scale;
					return;
				}

				int keyFrameStartIndex = keyFrameEndIndex - 1;

				ScaleKeyFrame startKeyFrame = animation->m_ScaleKeyFrames[keyFrameStartIndex];
				ScaleKeyFrame endKeyFrame = animation->m_ScaleKeyFrames[keyFrameEndIndex];

				float percent = (time - startKeyFrame.m_Time) / (endKeyFrame.m_Time - startKeyFrame.m_Time);
				float result = 0.0f;

				switch (endKeyFrame.n_EaseType)
				{
				case EasingType::Linear:
					result = percent;
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f); // white
					break;

				case EasingType::sineEaseIn:
					result = glm::sineEaseIn(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
					break;

				case EasingType::sineEaseOut:
					result = glm::sineEaseInOut(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
					break;

				case EasingType::sineEaseInOut:
					result = glm::sineEaseInOut(percent);
					gameObject->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					break;
				}

				glm::vec3 delta = endKeyFrame.m_Scale - startKeyFrame.m_Scale;
				gameObject->drawScale = startKeyFrame.m_Scale + delta * result;
			}
		}
	}

	for (cMesh* pChildMesh : meshToUpdate->vec_pChildMeshes) 
	{
		this->Update(pChildMesh); 
	}
}

void AnimationManager::UpdateAll(const std::vector<cMesh*>& meshesToUpdate)
{
	for (cMesh* gameObject : meshesToUpdate)
	{
		if (gameObject->m_Animation != nullptr)
		{
			Animation* animation = gameObject->m_Animation;
			double time = animation->m_Time;

			// Position
			if (animation->m_PositionKeyFrames.size() != 0)
			{


				// If there is only 1 key frame, use that one.
				// If we are equal to or greater than the time of the last key frame, use the last keyframe
				if (animation->m_PositionKeyFrames.size() == 1)
				{
					gameObject->drawPosition = animation->m_PositionKeyFrames[0].m_Position;
				}
				else
				{
					// Find our position keyframes (Start and End)
					int KeyFrameEndIndex = 0;
					for (; KeyFrameEndIndex < animation->m_PositionKeyFrames.size(); KeyFrameEndIndex++)
					{
						//std::cout << "time: " << time << " m_time: " << animation->m_PositionKeyFrames[KeyFrameEndIndex].m_Time << std::endl;
						if (animation->m_PositionKeyFrames[KeyFrameEndIndex].m_Time > time)
						{
							break;
						}
					}

					if (KeyFrameEndIndex >= animation->m_PositionKeyFrames.size())
					{
						// we are at or past the last key frame use the last keyframe only
						gameObject->drawPosition = animation->m_PositionKeyFrames[KeyFrameEndIndex - 1].m_Position;
						return;
					}
					int KeyFrameStartIndex = KeyFrameEndIndex - 1;


					PositionKeyFrame startKeyFrame = animation->m_PositionKeyFrames[KeyFrameStartIndex];
					PositionKeyFrame endKeyFrame = animation->m_PositionKeyFrames[KeyFrameEndIndex];

					float percent = (time - startKeyFrame.m_Time) / (endKeyFrame.m_Time - startKeyFrame.m_Time);
					float result = 0.f;
					switch (endKeyFrame.n_EaseType)
					{
					case EasingType::Linear:
						result = percent;
						//gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f); // white
						break;

					case EasingType::sineEaseIn:
						result = glm::sineEaseIn(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
						break;

					case EasingType::sineEaseOut:
						result = glm::sineEaseOut(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
						break;

					case EasingType::sineEaseInOut:
						result = glm::sineEaseInOut(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
						break;
					}

					glm::vec3 delta = endKeyFrame.m_Position - startKeyFrame.m_Position;
					//std::cout << delta.x << delta.y << delta.z << std::endl;
					gameObject->drawPosition = startKeyFrame.m_Position + delta * result;
				}
			}


			// Rotation
			if (animation->m_RotationKeyFrames.size() != 0)
			{
				if (animation->m_RotationKeyFrames.size() == 1)
				{
					gameObject->setDrawOrientation(animation->m_RotationKeyFrames[0].m_Rotation);
				}
				else
				{
					int keyFrameEndIndex = 0;
					for (; keyFrameEndIndex < animation->m_RotationKeyFrames.size(); keyFrameEndIndex++)
					{
						if (animation->m_RotationKeyFrames[keyFrameEndIndex].m_Time > time)
						{
							break;
						}
					}

					if (keyFrameEndIndex >= animation->m_RotationKeyFrames.size())
					{
						gameObject->setDrawOrientation(animation->m_RotationKeyFrames[keyFrameEndIndex - 1].m_Rotation);
						return;
					}

					int keyFrameStartIndex = keyFrameEndIndex - 1;

					RotationKeyFrame startKeyFrame = animation->m_RotationKeyFrames[keyFrameStartIndex];
					RotationKeyFrame endKeyFrame = animation->m_RotationKeyFrames[keyFrameEndIndex];

					float percent = (time - startKeyFrame.m_Time) / (endKeyFrame.m_Time - startKeyFrame.m_Time);
					float result = 0;

					switch (endKeyFrame.n_EaseType)
					{
					case EasingType::Linear:
						result = percent;
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f); // white
						break;

					case EasingType::sineEaseIn:
						result = glm::sineEaseIn(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
						break;

					case EasingType::sineEaseOut:
						result = glm::sineEaseOut(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
						break;

					case EasingType::sineEaseInOut:
						result = glm::sineEaseInOut(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
						break;

					}

					gameObject->setDrawOrientation(glm::slerp(startKeyFrame.m_Rotation, endKeyFrame.m_Rotation, result));
				}
			}

			// Scale 
			if (animation->m_ScaleKeyFrames.size() != 0)
			{
				if (animation->m_ScaleKeyFrames.size() == 1)
				{
					gameObject->drawScale = animation->m_ScaleKeyFrames[0].m_Scale;
				}
				else
				{
					int keyFrameEndIndex = 0;
					for (; keyFrameEndIndex < animation->m_ScaleKeyFrames.size(); keyFrameEndIndex++)
					{
						if (animation->m_ScaleKeyFrames[keyFrameEndIndex].m_Time > time)
						{
							break;
						}
					}

					if (keyFrameEndIndex >= animation->m_ScaleKeyFrames.size())
					{
						gameObject->drawScale = animation->m_ScaleKeyFrames[keyFrameEndIndex - 1].m_Scale;
						return;
					}

					int keyFrameStartIndex = keyFrameEndIndex - 1;

					ScaleKeyFrame startKeyFrame = animation->m_ScaleKeyFrames[keyFrameStartIndex];
					ScaleKeyFrame endKeyFrame = animation->m_ScaleKeyFrames[keyFrameEndIndex];

					float percent = (time - startKeyFrame.m_Time) / (endKeyFrame.m_Time - startKeyFrame.m_Time);
					float result = 0.0f;

					switch (endKeyFrame.n_EaseType)
					{
					case EasingType::Linear:
						result = percent;
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f); // white
						break;

					case EasingType::sineEaseIn:
						result = glm::sineEaseIn(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
						break;

					case EasingType::sineEaseOut:
						result = glm::sineEaseInOut(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
						break;

					case EasingType::sineEaseInOut:
						result = glm::sineEaseInOut(percent);
						gameObject->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
						break;
					}

					glm::vec3 delta = endKeyFrame.m_Scale - startKeyFrame.m_Scale;
					gameObject->drawScale = startKeyFrame.m_Scale + delta * result;
				}
			}
		}

		this->UpdateAll(gameObject->vec_pChildMeshes);
	}
}

float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float keyFrameTime)
{
	float scaleFactor = 0.0f;
	scaleFactor = (keyFrameTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
	return scaleFactor;
}

glm::mat4 AnimationManager::NodeAnimMatrix(NodeAnim* node, double keyFrameTime)
{
	glm::mat4 translation = glm::mat4(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);
	
	// position
	if (node->m_PositionKeyFrames.size() == 1)
	{
		translation = glm::translate(glm::mat4(1.0f), node->m_PositionKeyFrames[0].m_Position);
	}
	else
	{
		int p0Index = 0;
		for (int i = 0; i < node->m_PositionKeyFrames.size() - 1; i++)
		{
			if (keyFrameTime < node->m_PositionKeyFrames[i + 1].m_Time)
			{
				p0Index = i;
				break;
			}
		}

		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(node->m_PositionKeyFrames[p0Index].m_Time,
			node->m_PositionKeyFrames[p1Index].m_Time,
			keyFrameTime);
		//std::cout << scaleFactor << std::endl;
		glm::vec3 finalPosition = glm::mix(node->m_PositionKeyFrames[p0Index].m_Position, 
			node->m_PositionKeyFrames[p1Index].m_Position, 
			scaleFactor);

		translation = glm::translate(glm::mat4(1.0f), finalPosition);
	}

	// rotation
	if (node->m_RotationKeyFrames.size() == 1)
	{
		auto rotate = glm::normalize(node->m_RotationKeyFrames[0].m_Rotation);
		rotation = glm::toMat4(rotate);
	}
	else
	{
		int p0index = 0;
		for (int i = 0; i < node->m_RotationKeyFrames.size() - 1; i++)
		{
			if (keyFrameTime < node->m_RotationKeyFrames[i + 1].m_Time)
			{
				p0index = i;
				break;
			}
		}

		int p1Index = p0index + 1;
		float scaleFactor = GetScaleFactor(node->m_RotationKeyFrames[p0index].m_Time,
			node->m_RotationKeyFrames[p1Index].m_Time,
			keyFrameTime);
		//std::cout << scaleFactor << std::endl;
		glm::quat finalRotation = glm::slerp(node->m_RotationKeyFrames[p0index].m_Rotation,
			node->m_RotationKeyFrames[p1Index].m_Rotation,
			scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		rotation = glm::toMat4(finalRotation);
	}
	
	// scale
	if (node->m_ScaleKeyFrames.size() == 1)
	{
		scale = glm::scale(glm::mat4(1.0f), node->m_ScaleKeyFrames[0].m_Scale);
	}
	else
	{
		int p0Index = 0;
		for (int i = 0; i < node->m_ScaleKeyFrames.size() - 1; i++)
		{
			if (keyFrameTime < node->m_ScaleKeyFrames[i + 1].m_Time)
			{
				p0Index = i;
				break;
			}
		}

		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(node->m_ScaleKeyFrames[p0Index].m_Time,
			node->m_ScaleKeyFrames[p1Index].m_Time,
			keyFrameTime);
		//std::cout << scaleFactor << std::endl;
		glm::vec3 finalScale = glm::mix(node->m_ScaleKeyFrames[p0Index].m_Scale,
			node->m_ScaleKeyFrames[p1Index].m_Scale,
			scaleFactor);
		//std::cout << finalScale.x << ", " << finalScale.y << ", " << finalScale.z << std::endl;
		scale = glm::scale(glm::mat4(1.0f), finalScale);
	}

	return translation * rotation * scale;
}

void AnimationManager::CalculateMatrices(sModelDrawInfo* model, 
	CharacterAnimation* animation, 
	Node* node, 
	const glm::mat4& parentTransformation, 
	double keyFrameTime,
	glm::vec3 lookingAt)
{
	std::string nodeName = node->Name;

	glm::mat4 transformationMatrix = node->transformation;

	NodeAnim* nodeAnimation = animation->findNodeAnim(nodeName);

	if (nodeAnimation)
	{
		transformationMatrix = NodeAnimMatrix(nodeAnimation, keyFrameTime);
	}

	glm::mat4 globalTransformation = parentTransformation * transformationMatrix;

	if (nodeName == "mixamorig_Neck")
	{
		if (lookingAt != glm::vec3(0.0f))
		{
			globalTransformation *= glm::toMat4(glm::quatLookAt(lookingAt, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
	}

	// if there is a bone associated with this name assign the global transformation
	auto boneMapIt = model->BoneNameToIdMap.find(nodeName);

	if (boneMapIt != model->BoneNameToIdMap.end())
	{
		BoneInfo& boneInfo = model->BoneInfoVec[boneMapIt->second];
		boneInfo.FinalTransformation = model->GlobalInverseTransformation * globalTransformation * boneInfo.BoneOffset;
		boneInfo.GlobalTransformation = globalTransformation;
	}

	// calculate all children
	for (int i = 0; i < node->children.size(); ++i)
	{
		CalculateMatrices(model, animation, node->children[i], globalTransformation, keyFrameTime, lookingAt);
	}


}


void AnimationManager::UpdateCharacter(const cMesh* characterUpdate, int animation, sModelDrawInfo* model, double deltaTime, glm::vec3 lookingAt)
{
	
	
	if (characterUpdate != nullptr && characterUpdate->meshName == model->meshName)
	{
		glm::mat4 origin = glm::mat4(1.0f);
		glm::mat4 rootTransformation = glm::mat4(1.0f);
		double keyFrameTime;
		
		if (animation == -1)
		{
			keyFrameTime = characterUpdate->blendAnimation->currentTime;
			keyFrameTime += characterUpdate->blendAnimation->ticksPerSecond * deltaTime;
			keyFrameTime = fmod(keyFrameTime, characterUpdate->blendAnimation->duration);
			characterUpdate->blendAnimation->currentTime += characterUpdate->blendAnimation->ticksPerSecond * deltaTime;//keyFrameTime;

			CalculateMatrices(model, characterUpdate->blendAnimation, model->RootNode, rootTransformation, characterUpdate->blendAnimation->currentTime, lookingAt);
			return;
		}
		

		keyFrameTime = model->CharacterAnimations[animation]->currentTime;
		keyFrameTime += model->CharacterAnimations[animation]->ticksPerSecond * deltaTime;// / 20.0;
		keyFrameTime = fmod(keyFrameTime, model->CharacterAnimations[animation]->duration);
		model->CharacterAnimations[animation]->currentTime = keyFrameTime;

		
		//std::cout << model->meshName << " " << model->CharacterAnimations[animate]->Name << " " << model->CharacterAnimations.size() << std::endl;

		CalculateMatrices(model, model->CharacterAnimations[animation], model->RootNode, rootTransformation, keyFrameTime, lookingAt);
	}
}


void AnimationManager::BlendAnimations(cMesh* characterUpdate, 
	int animationOne, 
	int animationTwo, 
	sModelDrawInfo* model, 
	double deltaTime, 
	glm::vec3 lookingAt)
{
	CharacterAnimation* blendAnimtion = new CharacterAnimation();

	blendAnimtion->Name = "blendAnimtionation";
	blendAnimtion->duration = 250.0;
	blendAnimtion->ticksPerSecond = 1000.0;
	blendAnimtion->currentTime = 0.0;

	double keyFrameTime = model->CharacterAnimations[animationOne]->currentTime;

	for (int i = 0; i < model->CharacterAnimations[animationOne]->Channels.size(); i++)
	{
		NodeAnim* nodeAnim = new NodeAnim(model->CharacterAnimations[animationOne]->Channels[i]->Name);

		NodeAnim* prevNode = model->CharacterAnimations[animationOne]->findNodeAnim(nodeAnim->Name); 

		NodeAnim* nextNode = model->CharacterAnimations[animationTwo]->findNodeAnim(nodeAnim->Name);

		double nextTime = model->CharacterAnimations[animationTwo]->currentTime;

		if (prevNode && nextNode)
		{
			// position key frame previous
			if (prevNode->m_PositionKeyFrames.size() == 1)
			{
				nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(prevNode->m_PositionKeyFrames[0].m_Position, 0.0));
			}
			else
			{
				int p0Index = 0;
				for (int i = 0; i < prevNode->m_PositionKeyFrames.size() - 1; i++)
				{
					if (keyFrameTime < prevNode->m_PositionKeyFrames[i + 1].m_Time)
					{
						p0Index = i;
						break;
					}
				}

				int p1Index = p0Index + 1;
				float scaleFactor = GetScaleFactor(prevNode->m_PositionKeyFrames[p0Index].m_Time,
					prevNode->m_PositionKeyFrames[p1Index].m_Time,
					keyFrameTime);
				//std::cout << scaleFactor << std::endl;
				glm::vec3 finalPosition = glm::mix(prevNode->m_PositionKeyFrames[p0Index].m_Position,
					prevNode->m_PositionKeyFrames[p1Index].m_Position,
					scaleFactor);

				nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(finalPosition, 0.0));
			}

			// position key frame next
			if (nextNode->m_PositionKeyFrames.size() == 1)
			{
				nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(prevNode->m_PositionKeyFrames[0].m_Position, 0.0));
			}
			else
			{
				int p0Index = 0;
				for (int i = 0; i < nextNode->m_PositionKeyFrames.size() - 1; i++)
				{
					if (nextTime < nextNode->m_PositionKeyFrames[i + 1].m_Time)
					{
						p0Index = i;
						break;
					}
				}

				int p1Index = p0Index + 1;
				float scaleFactor = GetScaleFactor(nextNode->m_PositionKeyFrames[p0Index].m_Time,
					nextNode->m_PositionKeyFrames[p1Index].m_Time,
					nextTime);
				//std::cout << scaleFactor << std::endl;
				glm::vec3 finalPosition = glm::mix(nextNode->m_PositionKeyFrames[p0Index].m_Position,
					nextNode->m_PositionKeyFrames[p1Index].m_Position,
					scaleFactor);

				nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(finalPosition, blendAnimtion->duration));
			}
			//nodeAnim->m_PositionKeyFrames.emplace_back(PositionKeyFrame(nextNode->m_PositionKeyFrames[0].m_Position, blendAnimtion->duration));

			// rotation key frame prev
			if (prevNode->m_RotationKeyFrames.size() == 1)
			{
				nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(prevNode->m_RotationKeyFrames[0].m_Rotation, 0.0));
			}
			else
			{
				int p0index = 0;
				for (int i = 0; i < prevNode->m_RotationKeyFrames.size() - 1; i++)
				{
					if (keyFrameTime < prevNode->m_RotationKeyFrames[i + 1].m_Time)
					{
						p0index = i;
						break;
					}
				}

				int p1Index = p0index + 1;
				float scaleFactor = GetScaleFactor(prevNode->m_RotationKeyFrames[p0index].m_Time,
					prevNode->m_RotationKeyFrames[p1Index].m_Time,
					keyFrameTime);
				//std::cout << scaleFactor << std::endl;
				glm::quat finalRotation = glm::slerp(prevNode->m_RotationKeyFrames[p0index].m_Rotation,
					prevNode->m_RotationKeyFrames[p1Index].m_Rotation,
					scaleFactor);
				finalRotation = glm::normalize(finalRotation);
				
				nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(finalRotation, 0.0));
			}
			
			// rotation key frame next node
			if (nextNode->m_RotationKeyFrames.size() == 1)
			{
				nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(nextNode->m_RotationKeyFrames[0].m_Rotation, 0.0));
			}
			else
			{
				int p0index = 0;
				for (int i = 0; i < nextNode->m_RotationKeyFrames.size() - 1; i++)
				{
					if (nextTime < nextNode->m_RotationKeyFrames[i + 1].m_Time) 
					{
						p0index = i;
						break;
					}
				}

				int p1Index = p0index + 1;
				float scaleFactor = GetScaleFactor(nextNode->m_RotationKeyFrames[p0index].m_Time,
					nextNode->m_RotationKeyFrames[p1Index].m_Time,
					nextTime);
				//std::cout << scaleFactor << std::endl;
				glm::quat finalRotation = glm::slerp(nextNode->m_RotationKeyFrames[p0index].m_Rotation,
					nextNode->m_RotationKeyFrames[p1Index].m_Rotation,
					scaleFactor);
				finalRotation = glm::normalize(finalRotation);
				
				nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(finalRotation, blendAnimtion->duration));
			}

			//nodeAnim->m_RotationKeyFrames.emplace_back(RotationKeyFrame(nextNode->m_RotationKeyFrames[0].m_Rotation, blendAnimtion->duration));

			// scaling key frames previous
			if (prevNode->m_ScaleKeyFrames.size() == 1)
			{
				nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(prevNode->m_ScaleKeyFrames[0].m_Scale, 0.0));
			}
			else
			{
				int p0Index = 0;
				for (int i = 0; i < prevNode->m_ScaleKeyFrames.size() - 1; i++)
				{
					if (keyFrameTime < prevNode->m_ScaleKeyFrames[i + 1].m_Time)
					{
						p0Index = i;
						break;
					}
				}

				int p1Index = p0Index + 1;
				float scaleFactor = GetScaleFactor(prevNode->m_ScaleKeyFrames[p0Index].m_Time,
					prevNode->m_ScaleKeyFrames[p1Index].m_Time,
					keyFrameTime);
				
				glm::vec3 finalScale = glm::mix(prevNode->m_ScaleKeyFrames[p0Index].m_Scale,
					prevNode->m_ScaleKeyFrames[p1Index].m_Scale,
					scaleFactor);
				
				nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(finalScale, 0.0));
			}
			
			// scaling key frames next node
			if (nextNode->m_ScaleKeyFrames.size() == 1)
			{
				nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(nextNode->m_ScaleKeyFrames[0].m_Scale, 0.0));
			}
			else
			{
				int p0Index = 0;
				for (int i = 0; i < nextNode->m_ScaleKeyFrames.size() - 1; i++)
				{
					if (nextTime < nextNode->m_ScaleKeyFrames[i + 1].m_Time)
					{
						p0Index = i;
						break;
					}
				}

				int p1Index = p0Index + 1;
				float scaleFactor = GetScaleFactor(nextNode->m_ScaleKeyFrames[p0Index].m_Time,
					nextNode->m_ScaleKeyFrames[p1Index].m_Time,
					nextTime);
				
				glm::vec3 finalScale = glm::mix(nextNode->m_ScaleKeyFrames[p0Index].m_Scale,
					nextNode->m_ScaleKeyFrames[p1Index].m_Scale,
					scaleFactor);
				
				nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(finalScale, blendAnimtion->duration));
			}

			//nodeAnim->m_ScaleKeyFrames.emplace_back(ScaleKeyFrame(nextNode->m_ScaleKeyFrames[0].m_Scale, blendAnimtion->duration));

			blendAnimtion->Channels.emplace_back(nodeAnim);
		}
	}

	characterUpdate->blendAnimation = blendAnimtion; 
	characterUpdate->hasBlend = true;
}





