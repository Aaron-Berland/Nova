#pragma once
#ifndef HEXUTILITY_H
#define HEXUTILITY_H
#include <glm/glm.hpp>

#include <stb_perlin.h>

inline glm::vec3  VectorLerp(const glm::vec3 &v0, const glm::vec3  &v1, float value)
{
	return  v1*value + (1 - value) *v0;
}
struct PositionNormalColorVertexData
{
	float x, y, z, nx, ny, nz, r, g, b, a;
	PositionNormalColorVertexData(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec4 &color)
		: x(pos.x), y(pos.y), z(pos.z), nx(normal.x), ny(normal.y), nz(normal.z), r(color.x), g(color.y), b(color.z), a(color.w)
	{}


};

struct PositionUVUVVertexData
{
	float x, y, z, u0, v0, u1, v1;
	PositionUVUVVertexData(const glm::vec3 &pos, const glm::vec2&uv0, const glm::vec2 &uv1)
		: x(pos.x), y(pos.y), z(pos.z), u0(uv0.x), v0(uv0.y), u1(uv1.x), v1(uv1.y)
	{}
};
struct PositionUVVertexData
{
	float x, y, z, u, v;
	PositionUVVertexData(const glm::vec3&pos, const glm::vec2&uv)
		: x(pos.x), y(pos.y), z(pos.z), u(uv.x), v(uv.y)
	{}
};

struct PositionVertexData
{

	float x, y, z;
	PositionVertexData(const glm::vec3  &pos)
		: x(pos.x), y(pos.y), z(pos.z) {}
};


struct NoiseInfo
{
	float hStrength;
	float vStrength;
	float scale;
	NoiseInfo(float hs, float vs, float s) : hStrength(hs), vStrength(vs), scale(s) {}

};


inline static glm::vec3 SampleNoise3D(const NoiseInfo &noise, const glm::vec3 &p)
{
	glm::vec3 scaledp = noise.scale * p;
	float offsetx = stb_perlin_noise3(scaledp.x, scaledp.y, 0.0f, 0, 0, 0) * noise.hStrength;
	float offsety = stb_perlin_noise3(scaledp.y, 0.0f, -scaledp.x, 0, 0, 0) * noise.hStrength;
	return glm::vec3(offsetx, offsety, 0.0f);
}

inline static float SampleNoise(const NoiseInfo &noise, const glm::vec3 &p)
{
	glm::vec3 scaledp = noise.scale * p;
	return stb_perlin_noise3(0.0f, scaledp.x, -scaledp.y, 0, 0, 0) * noise.vStrength;
}

#endif