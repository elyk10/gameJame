#include "cGrassSystem.h"
#include <iostream>

cGrassSystem::cGrassSystem()
{
	this->m_timeLoop = false;
}

cGrassSystem::~cGrassSystem()
{

}

float grass_getRandomFloat(float min, float max)
{
	// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return (min + r);
}

glm::vec3 grass_getRandomVec3Float(glm::vec3 min, glm::vec3 max)
{
	glm::vec3 randPoint;
	randPoint.x = grass_getRandomFloat(min.x, max.x);
	randPoint.y = grass_getRandomFloat(min.y, max.y);
	randPoint.z = grass_getRandomFloat(min.z, max.z);
	return randPoint;
}

void cGrassSystem::InitializeGrass(sGrassInfo& initialState)
{
	this->m_GrassInfo = initialState;

	this->m_Blades.clear();
	this->m_Blades.reserve(initialState.numOfBlades);

	for (int i = 0; i < initialState.numOfBlades; i++)
	{
		cBlade newBlade;
		newBlade.position = grass_getRandomVec3Float(m_GrassInfo.minBound, m_GrassInfo.maxBound);
		newBlade.colour = glm::vec4(grass_getRandomVec3Float(m_GrassInfo.colourMin, m_GrassInfo.colourMax), 1.0f);
		newBlade.scale = grass_getRandomVec3Float(m_GrassInfo.scaleDifferenceMin, m_GrassInfo.scaleDifferenceMax);

		m_Blades.push_back(newBlade);
	}
}

void cGrassSystem::Update(double deltaTime)
{
	if (!this->m_timeLoop)
	{
		this->m_timePassed += deltaTime;
		if (this->m_timePassed >= 10.0)
		{
			this->m_timeLoop = true;
		}
	}
	else
	{
		this->m_timePassed -= deltaTime;
		if (this->m_timePassed <= 0.0)
		{
			this->m_timeLoop = false;
		}
	}
	
	/*this->m_timePassed += deltaTime;

	if (this->m_timePassed >= 10.0)
	{
		this->m_timePassed = fmod(this->m_timePassed, 10.0);
	}
	else if (this->m_timePassed <= 0.0)
	{
		this->m_timePassed = 10.0 + fmod(this->m_timePassed, 10.0);
	}*/

}
