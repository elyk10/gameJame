#include "cMesh.h"

// given a line segment ab and a  point c compute the closest point on the line ab
// also return t, where d(t) = a + t * (b - a)
glm::vec3 ClosestPoint(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC, float& t) 
{
	// project c onto ab
	t = glm::dot(pointC - pointA, pointB - pointA) / glm::dot(pointB - pointA, pointB - pointA);

	//clamp t to a 0-1 range. if t > 1 or t < 0 then the closest point is outside the line
	t = glm::clamp(t, 0.0f, 1.0f);

	// compute the projected position from the clamped t value
	return glm::vec3(pointA + t * (pointB - pointA));

}

//std::vector<glm::vec3> points;
//float shortestDistance = std::numeric_limits<float>::max();
//float pathRadius; // defined with the path
//glm::vec3 futurePosition; // determined using current position and velocity
//glm::vec3 targetPosition; // to be determined below
//// loop through all the points on the path
//
//for (int idx = 0; idx < points.size() - 1; idx++)
//{
//	// look at each segment
//	glm::vec3 a = points[idx];
//	glm::vec3 b = points[idx + 1];
//
//	float t(0.0f);
//	glm::vec3 closestPoint = ClosestPoint(a, b, futurePosition, t);
//
//	float distance = glm::distance(futurePosition, closestPoint);
//	if (distance < shortestDistance)
//	{
//		shortestDistance = distance;
//
//		targetPosition = closestPoint;
//
//		// move the target position along the line an amount scaled by the distance
//		// also scaling by velocity might be a good idea
//		targetPosition += glm::normalize(b - a) * shortestDistance;
//	}
//}
//
//// all said and done we only want to do anything
//// if we are projected to be outsude the line's radius
//if (shortestDistance > pathRadius)
//{
//	// do the seek thing
//	// seek the target position
//}