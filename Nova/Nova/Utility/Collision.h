#pragma once
#ifndef COLLISION_H
#define COLLISION_H
#include <Nova/Utility/Geometry.h>
#include <glm/glm.hpp>

namespace Nova
{
	constexpr float epsilon = 0.0001f;
	struct RayHitInfo
	{
		bool hasHitted;
		float distance;
		RayHitInfo(bool hitted = false, float d = std::numeric_limits<float>::max()) : hasHitted(hitted), distance(d) {}
	};




	inline RayHitInfo TriangleRayIntersection(const glm::vec3  &v0, const glm::vec3  &v1, const glm::vec3  &v2, const Ray &ray)
	{
		glm::vec3 edge1, edge2, tvec, pvec, qvec;
		float det, inv_det;
		edge1 = v1 - v0;
		edge2 = v2 - v0;
		pvec = glm::cross(ray.direction, edge2);
		det = glm::dot(edge1, pvec);
		if (det > -epsilon && det < epsilon)
			return RayHitInfo();
		inv_det = 1.0f / det;
		tvec = ray.origin - v0;
		float u = glm::dot(tvec, pvec) * inv_det;
		if (u < 0.0f || u > 1.0f)
			return RayHitInfo();
		qvec = glm::cross(tvec, edge1);
		float v = glm::dot(ray.direction, qvec) * inv_det;
		if (v < 0.0f || u + v > 1.0f)
			return RayHitInfo();

		float t = glm::dot(edge2, qvec) * inv_det;
		if (t > 0.0f)
		{
			return RayHitInfo(true, t);
		}

		return RayHitInfo();
	}


	inline bool AABBRayIntersectionTest(const AABB &aabbs, const Ray &ray)
	{
		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		tmin = (aabbs.bounds[ray.sign[0]].x - ray.origin.x) * ray.invdir.x;
		tmax = (aabbs.bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.invdir.x;
		tymin = (aabbs.bounds[ray.sign[1]].y - ray.origin.y) * ray.invdir.y;
		tymax = (aabbs.bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.invdir.y;

		if ((tmin > tymax) || (tymin > tmax))
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		tzmin = (aabbs.bounds[ray.sign[2]].z - ray.origin.z) * ray.invdir.z;
		tzmax = (aabbs.bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}

}

#endif