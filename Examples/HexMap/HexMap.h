#pragma once
#ifndef HEXMAP_H
#define HEXMAP_H
#include <glm/glm.hpp>
#include <Nova/Core.h>
#include <Nova/Renderer/Buffer.h>
#include <Nova/Renderer/VertexArray.h>
#include <array>
#include <vector>
#include <limits>
#include <unordered_set>
#include <bitset> 
#include <Nova/Utility/Collision.h>
#include <Nova/Utility/Geometry.h>
#include "HexMapBrush.h"
#include "HexUtility.h"
#include "HexTile.h"

class HexMapBrush;
constexpr unsigned int  MAX_VERTICES_PER_TILE = 645;
constexpr unsigned int  MAX_RIVER_VERTICES_PER_TILE = 36;
constexpr unsigned int	MAX_WATER_VERTICES_PER_TILE = 72;
constexpr unsigned int  MAX_SHORE_VERTICES_PER_TILE = 162;
constexpr unsigned int	MAX_ESTUARY_VERTICES_PER_TILE = 30;
constexpr unsigned int  CHUNK_SIZE = 5;
constexpr float solidFactor = 0.8f;
constexpr float waterSolidFactor = 0.6f;


struct HexTileRayHitInfo
{
	std::vector<HexTile>::size_type hexIndex;
	Nova::RayHitInfo hitInfo;
	HexTileRayHitInfo() : hitInfo(), hexIndex(0) {}
	bool HasHitted() const { return hitInfo.hasHitted; }

};

inline void CheckHexTileHit(const HexTile &tile,
	std::vector<HexTile>::size_type index,
	const std::vector<std::vector<PositionNormalColorVertexData>> &positions,
	const Nova::Ray &ray,
	HexTileRayHitInfo &hit);


struct HexMapMetrics
{
	HexTileMetrics tileMetrics;
	NoiseInfo noise;
	glm::vec3 hexMapOrigin;
	std::array<glm::vec3, 6> hexTileOffsets;
	std::vector<glm::vec4> colorPool;
	unsigned int width;
	unsigned int height;
	unsigned int maxElevation;
	unsigned int terracesPerSlope;
	unsigned int terraceSteps;
	HexMapMetrics(const HexTileMetrics &metrics, NoiseInfo info, glm::vec3 &origin, unsigned int w, unsigned int h, unsigned int elevation, unsigned int terraces, const std::vector<glm::vec4> &colors)
		: tileMetrics(metrics), noise(info), hexMapOrigin(origin), width(w), height(h), maxElevation(elevation), terracesPerSlope(terraces), terraceSteps(terracesPerSlope * 2 + 1), colorPool(colors) 
	{
		hexTileOffsets[0] = glm::vec3(0.0f, tileMetrics.outerRadius, 0.0f);
		hexTileOffsets[1] = glm::vec3(-tileMetrics.innerRadius, 0.5f*tileMetrics.outerRadius, 0.0f);
		hexTileOffsets[2] = glm::vec3(-tileMetrics.innerRadius, -0.5f*tileMetrics.outerRadius, 0.0f);
		hexTileOffsets[3] = glm::vec3(0.0f, -tileMetrics.outerRadius, 0.0f);
		hexTileOffsets[4] = glm::vec3(tileMetrics.innerRadius, -0.5f*tileMetrics.outerRadius, 0.0f);
		hexTileOffsets[5] = glm::vec3(tileMetrics.innerRadius, 0.5f*tileMetrics.outerRadius, 0.0f);
	}
	inline glm::vec3 TerraceLerp(const glm::vec3 &a, const glm::vec3 &b, unsigned int step) const
	{
		float h = (float)step / (float)(terraceSteps);
		glm::vec3 result = a;
		result.x += (b.x - a.x)*h;
		result.y += (b.y - a.y)*h;

		float v = (float)((step + 1) / 2) / (float)(terracesPerSlope + 1);
		result.z += (b.z - a.z)*v;

		return result;
	}

	inline glm::vec4 ColorTerraceLerp(const glm::vec4 &a, const glm::vec4 &b, unsigned int step) const
	{
		float h = (float)step / (float)(terraceSteps);
		return a + h*(b - a);
	}
};



class HexMap
{
public:
	HexMap(const HexMapMetrics &metrics);
	std::vector<HexTile>::size_type TileCount() const { return m_tiles.size(); }
	unsigned int GetMaxElevation() const { return m_hexMapMetrics.maxElevation; }
	
	HexTileRayHitInfo GetHexTileInfo(Nova::Ray &ray)
	{
		
		std::unordered_set<std::vector<std::vector<PositionNormalColorVertexData>>::size_type> chunksToCheck;
		for (decltype(m_chunkVertices.size()) i = 0; i < m_chunkVertices.size(); i++)
		{
			if (Nova::AABBRayIntersectionTest(m_chunkAABBs[i], ray))
				chunksToCheck.insert(i);

		}
		std::vector<HexTile>::size_type index = 0;
		HexTileRayHitInfo hexTileInfo;
		for (const auto &tile : m_tiles)
		{
			if(chunksToCheck.find(tile.chunkIndex) != chunksToCheck.end())
				CheckHexTileHit(tile, index, m_chunkVertices, ray, hexTileInfo);
			index++;
		}
		return hexTileInfo;
		
	}
	const HexTile& GetTile(std::vector<HexTile>::size_type index) { return m_tiles[index]; }
	void EditMap(const HexMapBrush &brush);
	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cTerrainVaoBegin() { return m_vao.cbegin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cTerrainVaoEnd() { return m_vao.cend(); }
	
	std::vector<Nova::Ref<Nova::VertexArray>>::iterator TerrainVaoBegin() { return m_vao.begin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::iterator TerrainVaoEnd() { return m_vao.end(); }


	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cRiverVaoBegin() { return m_riverVao.cbegin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cRiverVaoEnd() { return m_riverVao.cend(); }

	std::vector<Nova::Ref<Nova::VertexArray>>::iterator RiverVaoBegin() { return m_riverVao.begin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::iterator RiverVaoEnd() { return m_riverVao.end(); }


	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cWaterVaoBegin() { return m_waterVao.cbegin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cWaterVaoEnd() { return m_waterVao.cend(); }

	std::vector<Nova::Ref<Nova::VertexArray>>::iterator WaterVaoBegin() { return m_waterVao.begin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::iterator WaterVaoEnd() { return m_waterVao.end(); }


	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cShoreVaoBegin() { return m_shoreVao.cbegin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cShoreVaoEnd() { return m_shoreVao.cend(); }

	std::vector<Nova::Ref<Nova::VertexArray>>::iterator ShoreVaoBegin() { return m_shoreVao.begin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::iterator ShoreVaoEnd() { return m_shoreVao.end(); }

	std::vector<Nova::Ref<Nova::VertexArray>>::iterator EstuaryVaoBegin() { return m_estuaryVao.begin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::iterator EstuaryVaoEnd() { return m_estuaryVao.end(); }

	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cEstuaryVaoBegin() { return m_estuaryVao.cbegin(); }
	std::vector<Nova::Ref<Nova::VertexArray>>::const_iterator const cEstuaryVaoEnd() { return m_estuaryVao.cend(); }
	
private:
	void InitializeCPURenderData();
	void InitializeGPURenderData();
	void UpdateCPURenderData();
	void UpdateGPURenderData();
	void UpdateCPURenderData(const std::unordered_set<unsigned int> &chunks);
	void UpdateGPURenderData(const std::unordered_set<unsigned int> &chunks);
	void AddTile(unsigned int i, unsigned int j);
	void FillNeighborhood(unsigned int i, unsigned int j);
	void AddMainHexTile(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile);
	void AddMainHexTileWithoutRiver(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile);
	void AddMainHexTileWithRiver(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile);
	void AddMainHexTileUnderWater(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile);
	void AddMainHexTileDirectionOpenWater(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile, unsigned int direction);
	void AddMainHexTileDirectionWaterShore(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile, unsigned int direction);
	void AddMainHexTileDirectionEstuary(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, bool incomingRiver,
		const glm::vec3 &e0, const glm::vec3 &e1, const glm::vec3 &e2, const glm::vec3 &e3, const glm::vec3 &e4,
		const glm::vec3 &e5, const glm::vec3 &e6, const glm::vec3 &e7, const glm::vec3 &e8, const glm::vec3 &e9);

	void AddGradeTwoConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex , const HexTile &hextile, const glm::vec3& center, unsigned int direction);
	void AddGradeThreeConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex , const HexTile &hextile, const glm::vec3& center, unsigned int direction);
	void AddOrdenedGradeThreeConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &bottom, const HexTile &bottomTile,
		const glm::vec3 &right, const HexTile &rightTile,
		const glm::vec3 &left, const HexTile &leftTile);
	void AddSSFConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile,
		const glm::vec3 &right, const HexTile &rightTile,
		const glm::vec3 &left, const HexTile &leftTile);
	void AddSCConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile,
		const glm::vec3 &right, const HexTile &rightTile,
		const glm::vec3 &left, const HexTile &leftTile);
	void AddCSConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile,
		const glm::vec3 &right, const HexTile &rightTile,
		const glm::vec3 &left, const HexTile &leftTile);
	void AddSCBoundary(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile,
		const glm::vec3 &left, const HexTile &leftTile,
		const glm::vec3 &boundary, const glm::vec4 &boundaryColor);


	inline void AddVertex(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &position, const glm::vec4 &color, const glm::vec3 &normal /*= glm::vec3(0.0f, 0.0f, 1.0f)*/)
	{
		m_chunkVertices[chunkIndex].emplace_back(position, normal, color);
		const glm::bvec3 greater = glm::greaterThan(position, m_chunkAABBs[chunkIndex].bounds[1]);
		const glm::bvec3 less = glm::lessThan(position, m_chunkAABBs[chunkIndex].bounds[0]);
		for (unsigned int i = 0; i < 3; i++)
		{
			if (greater[i])
				m_chunkAABBs[chunkIndex].bounds[1][i] = position[i];
			else if (less[i])
				m_chunkAABBs[chunkIndex].bounds[0][i] = position[i];
		}

	}
	
	inline void AddTriangle(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, 
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, 
		const glm::vec4 &c0, const glm::vec4 &c1, const glm::vec4 &c2)
	{
		const glm::vec3 nv0 = v0 + SampleNoise3D(m_hexMapMetrics.noise, v0);
		const glm::vec3 nv1 = v1 + SampleNoise3D(m_hexMapMetrics.noise, v1);
		const glm::vec3 nv2 = v2 + SampleNoise3D(m_hexMapMetrics.noise, v2);
		const glm::vec3 normal = glm::cross(glm::normalize(nv1 - nv0), glm::normalize(nv2 - nv0));
		//unsigned int currentIndex = (unsigned int)m_chunkVertices[chunkIndex].size();
		AddVertex(chunkIndex, nv0, c0, normal);
		AddVertex(chunkIndex, nv1, c1, normal);
		AddVertex(chunkIndex, nv2, c2, normal);

	}

	inline void AddQuad(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, 
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3, 
		const glm::vec4 &c0, const glm::vec4 &c1, const glm::vec4 &c2, const glm::vec4 &c3)
	{
		AddTriangle(chunkIndex,v0, v1, v3, c0, c1, c3);
		AddTriangle(chunkIndex,v0, v3, v2, c0, c3, c2);
	}
	inline void AddTriangleWithouthNoise(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec4 &c0, const glm::vec4 &c1, const glm::vec4 &c2)
	{
		const glm::vec3 normal = glm::cross(glm::normalize(v1 - v0), glm::normalize(v2 - v0));
		AddVertex(chunkIndex,v0, c0, normal);
		AddVertex(chunkIndex,v1, c1, normal);
		AddVertex(chunkIndex,v2, c2, normal);

	}
	inline void AddRiverVertex(std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
		const glm::vec3 &position, const glm::vec2 &uv)
	{
		m_chunkRiverVertices[chunkIndex].emplace_back(position, uv);
	}
	inline void AddRiverTriangle(std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
		const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2)
	{
		const glm::vec3 nv0 = v0 + SampleNoise3D(m_hexMapMetrics.noise, v0);
		const glm::vec3 nv1 = v1 + SampleNoise3D(m_hexMapMetrics.noise, v1);
		const glm::vec3 nv2 = v2 + SampleNoise3D(m_hexMapMetrics.noise, v2);
		AddRiverVertex(chunkIndex, nv0, uv0);
		AddRiverVertex(chunkIndex, nv1, uv1);
		AddRiverVertex(chunkIndex, nv2, uv2);
	}
	inline void AddRiverQuad(std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex, float elevation, float neighborElevation,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
		const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec2 &uv3)
	{
		const glm::vec3 nv0 = glm::vec3(v0.x, v0.y, elevation);
		const glm::vec3 nv1 = glm::vec3(v1.x, v1.y, neighborElevation);
		const glm::vec3 nv2 = glm::vec3(v2.x, v2.y, elevation);
		const glm::vec3 nv3 = glm::vec3(v3.x, v3.y, neighborElevation);

		AddRiverTriangle(chunkIndex, nv0, nv1, nv3, uv0, uv1, uv3);
		AddRiverTriangle(chunkIndex, nv0, nv3, nv2, uv0, uv3, uv2);
	}
	inline void AddRiverQuad(std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex, float elevation,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
		const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec2 &uv3)
	{
		AddRiverQuad(chunkIndex, elevation, elevation, v0, v1, v2, v3, uv0, uv1, uv2, uv3);
	}

	inline void AddWaterQuad(std::vector<std::vector<PositionVertexData>>::size_type chunkIndex,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3)
	{
		AddWaterTriangle(chunkIndex, v0, v1, v3);
		AddWaterTriangle(chunkIndex, v0, v3, v2);
	}
	inline void AddWaterTriangle(std::vector<std::vector<PositionVertexData>>::size_type chunkIndex,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
	{
		const glm::vec3 nv0 = v0 + SampleNoise3D(m_hexMapMetrics.noise, v0);
		const glm::vec3 nv1 = v1 + SampleNoise3D(m_hexMapMetrics.noise, v1);
		const glm::vec3 nv2 = v2 + SampleNoise3D(m_hexMapMetrics.noise, v2);
		AddWaterVertex(chunkIndex, nv0);
		AddWaterVertex(chunkIndex, nv1);
		AddWaterVertex(chunkIndex, nv2);
	}
	inline void AddWaterVertex(std::vector<std::vector<PositionVertexData>>::size_type chunkIndex,
		const glm::vec3 &position)
	{
		m_chunkWaterVertices[chunkIndex].emplace_back(position);
	}


	inline void AddWaterfall(std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex, 
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
		float y0, float y1, float waterY, 
		const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec2 &uv3)
	{
		const glm::vec3 nv0 = glm::vec3(v0.x, v0.y, y0) + SampleNoise3D(m_hexMapMetrics.noise, v0);
		glm::vec3 nv1 = glm::vec3(v1.x, v1.y, y1) + SampleNoise3D(m_hexMapMetrics.noise, v1);
		const glm::vec3 nv2 = glm::vec3(v2.x, v2.y, y0) + SampleNoise3D(m_hexMapMetrics.noise, v2);
		glm::vec3 nv3 = glm::vec3(v3.x, v3.y, y1) + SampleNoise3D(m_hexMapMetrics.noise, v3);
		float t = (waterY - y1) / (y0 - y1);
		nv1 = VectorLerp(nv1, nv0, t);
		nv3 = VectorLerp(nv3, nv2, t);
		
		AddUVUnperturbedTriangle(m_chunkRiverVertices, chunkIndex, nv0, nv1, nv3, uv0, uv1, uv3);
		AddUVUnperturbedTriangle(m_chunkRiverVertices, chunkIndex, nv0, nv3, nv2, uv0, uv3, uv2);
	}
	inline void AddUVUnPerturbedQuad(std::vector<std::vector<PositionUVVertexData>> &chunkList,
		std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
		const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec2 &uv3)
	{

		AddUVUnperturbedTriangle(chunkList, chunkIndex, v0, v1, v3, uv0, uv1, uv3);
		AddUVUnperturbedTriangle(chunkList, chunkIndex, v0, v3, v2, uv0, uv3, uv2);

	}

	inline void AddUVUnperturbedTriangle(std::vector<std::vector<PositionUVVertexData>> &chunkList,
		std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
		const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 & uv2)
	{
		AddUVVertex(chunkList, chunkIndex, v0, uv0);
		AddUVVertex(chunkList, chunkIndex, v1, uv1);
		AddUVVertex(chunkList, chunkIndex, v2, uv2);

	}


	inline void AddUVQuad(	std::vector<std::vector<PositionUVVertexData>> &chunkList,
									std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
									const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
									const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec2 &uv3)
	{
		AddUVTriangle(chunkList, chunkIndex, v0, v1, v3, uv0, uv1, uv3);
		AddUVTriangle(chunkList, chunkIndex, v0, v3, v2, uv0, uv3, uv2);
	}
	inline void AddUVTriangle(	std::vector<std::vector<PositionUVVertexData>> &chunkList,
										std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
										/*std::vector<std::vector<unsigned int>> &chunkTriangles, */
										const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
										const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 & uv2)
	{
		const glm::vec3 nv0 = v0 + SampleNoise3D(m_hexMapMetrics.noise, v0);
		const glm::vec3 nv1 = v1 + SampleNoise3D(m_hexMapMetrics.noise, v1);
		const glm::vec3 nv2 = v2 + SampleNoise3D(m_hexMapMetrics.noise, v2);
		AddUVVertex(chunkList, chunkIndex, nv0, uv0);
		AddUVVertex(chunkList, chunkIndex, nv1, uv1);
		AddUVVertex(chunkList, chunkIndex, nv2, uv2);

	}
	inline void AddUVVertex(std::vector<std::vector<PositionUVVertexData>> &chunkList,
									std::vector<std::vector<PositionUVVertexData>>::size_type chunkIndex,
									const glm::vec3 &position,
									const glm::vec2 &uv)
	{
		chunkList[chunkIndex].emplace_back(position, uv);
	}

	inline void AddUVUVQuad(std::vector<std::vector<PositionUVUVVertexData>> &chunkList,
		std::vector<std::vector<PositionUVUVVertexData>>::size_type chunkIndex,
		/*std::vector<std::vector<unsigned int>> &chunkTriangles,*/
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
		const glm::vec2 &uv00, const glm::vec2 &uv01, const glm::vec2 &uv02, const glm::vec2 &uv03,
		const glm::vec2 &uv10, const glm::vec2 &uv11, const glm::vec2 &uv12, const glm::vec2 &uv13)
	{
		AddUVUVTriangle(chunkList, chunkIndex, v0, v1, v3, uv00, uv01, uv03, uv10, uv11, uv13);
		AddUVUVTriangle(chunkList, chunkIndex, v0, v3, v2, uv00, uv03, uv02, uv10, uv13, uv12);
	}


	inline void AddUVUVTriangle(std::vector<std::vector<PositionUVUVVertexData>> &chunkList,
		std::vector<std::vector<PositionUVUVVertexData>>::size_type chunkIndex,
		const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
		const glm::vec2 &uv00, const glm::vec2 &uv01, const glm::vec2 &uv02,
		const glm::vec2 &uv10, const glm::vec2 &uv11, const glm::vec2 &uv12)
	{
		const glm::vec3 nv0 = v0 + SampleNoise3D(m_hexMapMetrics.noise, v0);
		const glm::vec3 nv1 = v1 + SampleNoise3D(m_hexMapMetrics.noise, v1);
		const glm::vec3 nv2 = v2 + SampleNoise3D(m_hexMapMetrics.noise, v2);
		AddUVUVVertex(chunkList, chunkIndex, nv0, uv00, uv10);
		AddUVUVVertex(chunkList, chunkIndex, nv1, uv01, uv11);
		AddUVUVVertex(chunkList, chunkIndex, nv2, uv02, uv12);
	}

	inline void AddUVUVVertex(std::vector<std::vector<PositionUVUVVertexData>> &chunkList,
		std::vector<std::vector<PositionUVUVVertexData>>::size_type chunkIndex,
		const glm::vec3 &position,
		const glm::vec2 &uv0,
		const glm::vec2 &uv1)
	{
		chunkList[chunkIndex].emplace_back(position, uv0,uv1);
	}


	inline void MarkChunkForEditing(int x, int y, const HexMapBrush &brush, std::unordered_set<unsigned int> &chunksModified) {
		bool needUpdate = false;
		if (y < 0 || y >= (int)m_hexMapMetrics.height*(int)CHUNK_SIZE)
			return;
		int i = x + y / 2;
		if (i < 0 || i >= (int)m_hexMapMetrics.width*(int)CHUNK_SIZE)
			return;
		unsigned int tileIndex = y + (int)m_hexMapMetrics.height*(int)CHUNK_SIZE*i;
		HexTile &tile = m_tiles[tileIndex];

		if (brush.CanColorEdit())
		{
			const glm::vec4 &brushColor = brush.GetColor(brush.GetCurrentColorIndex());
			if (!glm::all(glm::equal(brushColor, tile.color)))
			{
				tile.color = brushColor;
				needUpdate = true;
			}
		}
		if (brush.CanElevationEdit())
		{
			unsigned int brushElevation = brush.GetCurrentElevation();
			if (brushElevation != tile.elevation)
			{
				tile.elevation = brushElevation;
				if (tile.featuresMask[outGoingRiverBit])
				{
					HexTile &neighbor = m_tiles[tile.neighbors[tile.outGoingRiver]];
					if (!IsValidRiverDestination(tile,neighbor))
						RemoveOutGoingRiverFromTile(static_cast<decltype(m_tiles.size())>(tileIndex), chunksModified);
				}
				if (tile.featuresMask[inComingRiverBit])
				{
					HexTile &neighbor = m_tiles[tile.neighbors[tile.inComingRiver]];
					if (!IsValidRiverDestination(neighbor,tile))
						RemoveInComingRiverFromTile(static_cast<decltype(m_tiles.size())>(tileIndex), chunksModified);
				}
				tile.center.z = tile.elevation * m_hexMapMetrics.tileMetrics.elevationStep + SampleNoise(m_hexMapMetrics.noise, tile.center);
				needUpdate = true;
			}
		}
		if (brush.CanWaterEdit())
		{
			unsigned int brushWaterLevel = brush.GetCurrentWaterLevel();
			if (tile.waterLevel != brushWaterLevel)
			{
				tile.waterLevel = brushWaterLevel;
				if (tile.featuresMask[outGoingRiverBit])
				{
					HexTile &neighbor = m_tiles[tile.neighbors[tile.outGoingRiver]];
					if (!IsValidRiverDestination(tile, neighbor))
						RemoveOutGoingRiverFromTile(static_cast<decltype(m_tiles.size())>(tileIndex), chunksModified);
				}
				if (tile.featuresMask[inComingRiverBit])
				{
					HexTile &neighbor = m_tiles[tile.neighbors[tile.inComingRiver]];
					if (!IsValidRiverDestination(neighbor, tile))
						RemoveInComingRiverFromTile(static_cast<decltype(m_tiles.size())>(tileIndex), chunksModified);
				}
				needUpdate = true;
			}
		}
		if (needUpdate)
		{
			chunksModified.insert(tile.chunkIndex);
			for (unsigned int i = 0; i < 6; i++)
			{
				int neightIndex = tile.neighbors[i];
				if (neightIndex >= 0)
					chunksModified.insert(m_tiles[neightIndex].chunkIndex);

			}
		}
	}
	inline void RemoveInComingRiverFromTile(std::vector<HexTile>::size_type index, std::unordered_set<unsigned int> &chunksModified)
	{
		HexTile &tile = m_tiles[index];
		if (tile.featuresMask[inComingRiverBit])
		{

			tile.featuresMask[inComingRiverBit] = false;
			HexTile &ntile = m_tiles[tile.neighbors[tile.inComingRiver]];
			ntile.featuresMask[outGoingRiverBit] = false;
			chunksModified.insert(tile.chunkIndex);
			if (tile.chunkIndex != ntile.chunkIndex)
				chunksModified.insert(ntile.chunkIndex);

		}
	}
	inline void RemoveOutGoingRiverFromTile(std::vector<HexTile>::size_type index, std::unordered_set<unsigned int> &chunksModified)
	{
		HexTile &tile = m_tiles[index];
		if (tile.featuresMask[outGoingRiverBit])
		{

			tile.featuresMask[outGoingRiverBit] = false;
			HexTile &ntile = m_tiles[tile.neighbors[tile.outGoingRiver]];
			ntile.featuresMask[inComingRiverBit] = false;
			chunksModified.insert(tile.chunkIndex);
			if (tile.chunkIndex != ntile.chunkIndex)
				chunksModified.insert(ntile.chunkIndex);

		}
	}
	inline void RemoveRiversFromTile(std::vector<HexTile>::size_type index, std::unordered_set<unsigned int> &chunksModified)
	{
		RemoveInComingRiverFromTile(index, chunksModified);
		RemoveOutGoingRiverFromTile(index, chunksModified);
	}
	inline void SetOutGoingRiver(unsigned int direction, std::vector<HexTile>::size_type index, std::unordered_set<unsigned int> &chunksModified)
	{
		HexTile &tile = m_tiles[index];
		if (tile.featuresMask[outGoingRiverBit] && tile.outGoingRiver == direction)
			return;
		
		if (tile.neighbors[direction] == -1)
			return;
		HexTile &neighbor = m_tiles[tile.neighbors[direction]];
		if (!IsValidRiverDestination(tile, neighbor))
			return;
		RemoveOutGoingRiverFromTile(index, chunksModified);
		if (tile.featuresMask[inComingRiverBit] && tile.inComingRiver == direction)
			RemoveInComingRiverFromTile(index, chunksModified);
		tile.featuresMask[outGoingRiverBit] = true;
		tile.outGoingRiver = direction;
		chunksModified.insert(tile.chunkIndex);
		RemoveInComingRiverFromTile(static_cast<decltype(m_tiles.size())>(tile.neighbors[direction]), chunksModified);
		neighbor.featuresMask[inComingRiverBit] = true;
		neighbor.inComingRiver = (direction + 3) % 6;
		chunksModified.insert(neighbor.chunkIndex);

	}
	


private:
	//Map Data
	HexMapMetrics m_hexMapMetrics;
	std::vector<HexTile> m_tiles;
	std::vector<Nova::AABB> m_chunkAABBs;


	std::vector<std::vector<PositionNormalColorVertexData>> m_chunkVertices;
	std::vector<std::vector<PositionUVVertexData>> m_chunkRiverVertices;
	std::vector<std::vector<PositionVertexData>> m_chunkWaterVertices;
	std::vector<std::vector<PositionUVVertexData>> m_chunkShoreVertices;
	std::vector<std::vector<PositionUVUVVertexData>> m_chunkEstuaryVertices;


	std::vector<Nova::Ref<Nova::VertexBuffer>> m_vbo;
	std::vector<Nova::Ref<Nova::VertexArray>>  m_vao;

	std::vector<Nova::Ref<Nova::VertexBuffer>> m_riverVbo;
	std::vector<Nova::Ref<Nova::VertexArray>>  m_riverVao;
	
	std::vector<Nova::Ref<Nova::VertexBuffer>> m_waterVbo;
	std::vector<Nova::Ref<Nova::VertexArray>>  m_waterVao;

	std::vector<Nova::Ref<Nova::VertexBuffer>> m_shoreVbo;
	std::vector<Nova::Ref<Nova::VertexArray>>  m_shoreVao;

	std::vector<Nova::Ref<Nova::VertexBuffer>> m_estuaryVbo;
	std::vector<Nova::Ref<Nova::VertexArray>>  m_estuaryVao;



};


inline void CheckHexTileHit(const HexTile &tile, std::vector<HexTile>::size_type index,  
							const std::vector<std::vector<PositionNormalColorVertexData>> &vertices, 
							const Nova::Ray &ray,
							HexTileRayHitInfo &hexTileHitInfo)
{
	for (unsigned int i = tile.firstVertexIndex; i < (tile.firstVertexIndex + tile.vertexCount); )
	{
		glm::vec3 v0(vertices[tile.chunkIndex][i].x, vertices[tile.chunkIndex][i].y, vertices[tile.chunkIndex][i].z);
		i++;
		glm::vec3 v1(vertices[tile.chunkIndex][i].x, vertices[tile.chunkIndex][i].y, vertices[tile.chunkIndex][i].z);
		i++;
		glm::vec3 v2(vertices[tile.chunkIndex][i].x, vertices[tile.chunkIndex][i].y, vertices[tile.chunkIndex][i].z);
		i++;
		Nova::RayHitInfo tempInfo = Nova::TriangleRayIntersection(v0, v1, v2, ray);
		if (tempInfo.hasHitted && hexTileHitInfo.hitInfo.distance > tempInfo.distance)
		{
			hexTileHitInfo.hitInfo.distance = tempInfo.distance;
			hexTileHitInfo.hitInfo.hasHitted = true;
			hexTileHitInfo.hexIndex = index;
		}
	}
}


#endif