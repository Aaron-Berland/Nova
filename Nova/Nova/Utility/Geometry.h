#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm/glm.hpp>
namespace Nova
{

	struct Ray
	{

		glm::vec3 origin;
		glm::vec3 direction;
		glm::vec3 invdir;
		int sign[3];
		Ray() : origin(glm::vec3(0.0f, 0.0f, 0.0f)), direction(glm::vec3(1.0f, 0.0f, 0.0f)) {}

		Ray(const glm::vec3 &p, const glm::vec3 &d) :
			origin(p), direction(d), invdir(glm::vec3(1 / d.x, 1 / d.y, 1 / d.z)), sign{ (invdir.x < 0) , (invdir.y < 0) ,(invdir.z < 0) }
		{
		}


	};

	struct AABB
	{
		glm::vec3 bounds[2];
		AABB(const glm::vec3 &min, const glm::vec3 &max) {
			bounds[0] = min;
			bounds[1] = max;
		}
	};

}
#endif