#pragma once
#include "OpenGLCommon.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "cMesh.h"


struct cBlade
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(0.0f);
	glm::vec4 colour = glm::vec4(0.0f);
};


class cGrassSystem
{
public:
	cGrassSystem();
	~cGrassSystem();

	struct sGrassInfo
	{
		unsigned int numOfBlades = 0;
		float separationDistance = 0.01f;

		glm::vec3 scaleDifferenceMin = glm::vec3(0.0f);
		glm::vec3 scaleDifferenceMax = glm::vec3(0.0f);

		glm::vec3 constantForce = glm::vec3(0.0f); // wind direction and speed

		// area grass can spawn
		glm::vec3 minBound = glm::vec3(0.0f);
		glm::vec3 maxBound = glm::vec3(0.0f);

		glm::vec4 colourMin = glm::vec4(0.0f);
		glm::vec4 colourMax = glm::vec4(0.0f);

		cMesh* surface = nullptr; // not implemented yet. could be used for hair and grass on hills

	};

	void InitializeGrass(sGrassInfo& initialState);

	void Update(double deltaTime);

	

	std::vector<cBlade> m_Blades;

	sGrassInfo m_GrassInfo;

	double m_timePassed;
	bool m_timeLoop;

};
