#include "HexMap.h"
#include <cmath>
#include <Nova/Core.h>
#include <Nova/Log.h>
#include <stb_perlin.h>
#include <Nova/Core/Benchmarking.h>


HexMap::HexMap(const HexMapMetrics &metrics) : m_hexMapMetrics(metrics)
{
	
	m_tiles.reserve(m_hexMapMetrics.width* m_hexMapMetrics.height*CHUNK_SIZE*CHUNK_SIZE);
	for (unsigned int i = 0; i < m_hexMapMetrics.width*CHUNK_SIZE; i++)
		for (unsigned int j = 0; j < m_hexMapMetrics.height*CHUNK_SIZE; j++)
				AddTile(i, j);

	//Add neighbors on a second pass
	for (unsigned int i = 0; i < m_hexMapMetrics.width*CHUNK_SIZE; i++)
		for (unsigned int j = 0; j < m_hexMapMetrics.height*CHUNK_SIZE; j++)
			FillNeighborhood(i, j);

	

	InitializeCPURenderData();
	UpdateCPURenderData();

	InitializeGPURenderData();
	UpdateGPURenderData();
	
}

void HexMap::EditMap(const HexMapBrush &brush)
{
	std::unordered_set<unsigned int> chunksModified;
	HexTile &centraltile = m_tiles[brush.GetCurrentTileIndex()];
	int xCenter = centraltile.HexCoord[0];
	int yCenter = centraltile.HexCoord[1];
	for (int r = 0, y = yCenter - brush.GetCurrentBrushSize(); y <= yCenter; r++, y++)
		for (int x = xCenter - r; x <= xCenter + (int)brush.GetCurrentBrushSize(); x++)
			MarkChunkForEditing(x, y, brush, chunksModified);

	for (int r = 0, y = yCenter + brush.GetCurrentBrushSize(); y > yCenter; y--, r++)
		for (int x = xCenter - brush.GetCurrentBrushSize(); x <= xCenter + r; x++)
			MarkChunkForEditing(x, y, brush, chunksModified);

	if (brush.CanRiverEdit())
	{
		if (brush.IsDragging() && brush.GetRiverEditingMode() == RiverEditingMode::Adding)
		{
			auto current = brush.GetCurrentTileIndex();
			auto prev = brush.GetPrevTileIndex();
			unsigned int direction;
			if (AreNeighbors(current, m_tiles[current], prev, m_tiles[prev], direction))
			{

				SetOutGoingRiver(direction, brush.GetPrevTileIndex(), chunksModified);

			}
		}
		else if (brush.GetRiverEditingMode() == RiverEditingMode::Removing)
		{
			RemoveRiversFromTile(brush.GetCurrentTileIndex(), chunksModified);
		}
	}

	
	
	if (chunksModified.size() > 0)
	{
			UpdateCPURenderData(chunksModified);
			UpdateGPURenderData(chunksModified);
	}

}

void HexMap::AddTile(unsigned int i, unsigned int j)
{
	glm::vec4 tileColor = m_hexMapMetrics.colorPool[std::rand() % m_hexMapMetrics.colorPool.size()];
	int elevation = std::rand() % (m_hexMapMetrics.maxElevation + 1);
	glm::vec3 center = m_hexMapMetrics.hexMapOrigin + (float)(i + 0.5f*(j & 0x1))*glm::vec3(2.0f*m_hexMapMetrics.tileMetrics.innerRadius, 0.0f, 0.0f) + (float)j*glm::vec3(0.0f, 1.5f*m_hexMapMetrics.tileMetrics.outerRadius, 0.0f);
	center.z += elevation * m_hexMapMetrics.tileMetrics.elevationStep + SampleNoise(m_hexMapMetrics.noise,center);
	m_tiles.emplace_back(center,i, j, i / CHUNK_SIZE + (j / CHUNK_SIZE) * m_hexMapMetrics.width,elevation, tileColor);
}

void HexMap::FillNeighborhood(unsigned int i, unsigned int j)
{
	unsigned int index = i*m_hexMapMetrics.height*CHUNK_SIZE + j;
	if (i > 0)
	{
		m_tiles[index].neighbors[1] = index - m_hexMapMetrics.height*CHUNK_SIZE;
		m_tiles[index - m_hexMapMetrics.height*CHUNK_SIZE].neighbors[4] = index;
	}
	if (j > 0)
	{
		if ((j & 1) == 0)
		{
			m_tiles[index].neighbors[3] = index - 1;
			m_tiles[index - 1].neighbors[0] = index;
			if (i > 0)
			{
				m_tiles[index].neighbors[2] = index - 1 - m_hexMapMetrics.height*CHUNK_SIZE;
				m_tiles[index - 1 - m_hexMapMetrics.height*CHUNK_SIZE].neighbors[5] = index;
			}
		}
		else
		{
			m_tiles[index].neighbors[2] = index - 1;
			m_tiles[index - 1].neighbors[5] = index;
			if (i < m_hexMapMetrics.width*CHUNK_SIZE - 1)
			{
				m_tiles[index].neighbors[3] = index - 1 + m_hexMapMetrics.height*CHUNK_SIZE;
				m_tiles[index - 1 + m_hexMapMetrics.height*CHUNK_SIZE].neighbors[0] = index;
			}
		}
	}
}

void HexMap::AddMainHexTile(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile)
{
	
	if (hexTile.HasRiver())
		AddMainHexTileWithRiver(chunkIndex, pos, hexTile);
	else
		AddMainHexTileWithoutRiver(chunkIndex, pos, hexTile);

	if (hexTile.IsUnderWater())
		AddMainHexTileUnderWater(chunkIndex,pos,hexTile);

}

void HexMap::AddMainHexTileWithoutRiver(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile)
{
	for (decltype(m_hexMapMetrics.hexTileOffsets.size()) i = 0; i < m_hexMapMetrics.hexTileOffsets.size(); i++)
	{
		auto next = (i + 1) % m_hexMapMetrics.hexTileOffsets.size();
		const glm::vec3 v1 = pos + m_hexMapMetrics.hexTileOffsets[i] * m_hexMapMetrics.tileMetrics.solidRegionFactor;
		const glm::vec3 v2 = pos + m_hexMapMetrics.hexTileOffsets[next] * m_hexMapMetrics.tileMetrics.solidRegionFactor;
		const glm::vec3 midv1 = (3.0f / 4.0f) * v1 + (1.0f / 4.0f) * v2;
		const glm::vec3 midv2 = (1.0f / 2.0f) * v1 + (1.0f / 2.0f) * v2;
		const glm::vec3 midv3 = (1.0f / 4.0f) * v1 + (3.0f / 4.0f) * v2;
		AddTriangle(chunkIndex,
			pos,
			v1,
			midv1,
			hexTile.color,
			hexTile.color,
			hexTile.color);
		AddTriangle(chunkIndex,
			pos,
			midv1,
			midv2,
			hexTile.color,
			hexTile.color,
			hexTile.color);
		AddTriangle(chunkIndex,
			pos,
			midv2,
			midv3,
			hexTile.color,
			hexTile.color,
			hexTile.color);
		AddTriangle(chunkIndex,
			pos,
			midv3,
			v2,
			hexTile.color,
			hexTile.color,
			hexTile.color);
	}
}

void HexMap::AddMainHexTileWithRiver(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile)
{
	
	for (unsigned int i = 0; i < m_hexMapMetrics.hexTileOffsets.size(); i++)
	{
		unsigned int next = (i + 1) % m_hexMapMetrics.hexTileOffsets.size();
		const glm::vec3 v1 = pos + m_hexMapMetrics.hexTileOffsets[i] * solidFactor;
		const glm::vec3 v2 = pos + m_hexMapMetrics.hexTileOffsets[next] * solidFactor;
		const glm::vec3 midv1 = (3.0f / 4.0f) * v1 + (1.0f / 4.0f) * v2;
		glm::vec3 midv2 = (1.0f / 2.0f) * v1 + (1.0f / 2.0f) * v2;
		const glm::vec3 midv3 = (1.0f / 4.0f) * v1 + (3.0f / 4.0f) * v2;

		if (hexTile.HasRiverThroughEdge(i))
		{
			
			midv2.z = (hexTile.elevation + m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
			if (hexTile.HasRiverBeginOrEnd())
			{
				
				const glm::vec3 m0 = VectorLerp(pos , v1, 0.5f);
				const glm::vec3 m1 = VectorLerp(pos , midv1, 0.5f);
				glm::vec3 m2 = VectorLerp(pos , midv2, 0.5f);
				const glm::vec3 m3 = VectorLerp(pos , midv3, 0.5f);
				const glm::vec3 m4 = VectorLerp(pos , v2, 0.5f);
				m2.z = midv2.z;
				AddQuad(chunkIndex, m0, v1, m1, midv1, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, m1, midv1, m2, midv2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, m2, midv2, m3, midv3, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, m3, midv3, m4, v2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddTriangle(chunkIndex, pos , m0, m1, hexTile.color, hexTile.color, hexTile.color);
				AddTriangle(chunkIndex, pos , m1, m2, hexTile.color, hexTile.color, hexTile.color);
				AddTriangle(chunkIndex, pos , m2, m3, hexTile.color, hexTile.color, hexTile.color);
				AddTriangle(chunkIndex, pos , m3, m4, hexTile.color, hexTile.color, hexTile.color);
				if (!hexTile.IsUnderWater())
				{
					float riverElevation = (hexTile.elevation + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
					if (hexTile.HasIncomingRiver())
					{
						AddRiverTriangle(chunkIndex, glm::vec3(pos.x, pos.y, riverElevation), glm::vec3(m1.x, m1.y, riverElevation), glm::vec3(m3.x, m3.y, riverElevation),
							glm::vec2(0.5f, 0.4f), glm::vec2(0.0f, 0.2f), glm::vec2(1.0f, 0.2f));
						AddRiverQuad(chunkIndex, riverElevation, m1, midv1, m3, midv3, glm::vec2(0.0f, 0.2f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.2f), glm::vec2(1.0f, 0.0f));
					}
					else
					{

						AddRiverTriangle(chunkIndex, glm::vec3(pos.x, pos.y, riverElevation), glm::vec3(m1.x, m1.y, riverElevation), glm::vec3(m3.x, m3.y, riverElevation),
							glm::vec2(0.5f, 0.4f), glm::vec2(1.0f, 0.6f), glm::vec2(0.0f, 0.6f));
						AddRiverQuad(chunkIndex, riverElevation, m1, midv1, m3, midv3, glm::vec2(1.0f, 0.6f), glm::vec2(1.0f, 0.8f), glm::vec2(0.0f, 0.6f), glm::vec2(0.0f, 0.8f));
					}
				}
				

			}
			else
			{
				unsigned int nnext = (i + 2) % m_hexMapMetrics.hexTileOffsets.size();
				unsigned int prev = (i + 5) % m_hexMapMetrics.hexTileOffsets.size();
				glm::vec3 centerL, centerR;
				if (hexTile.HasRiverThroughEdge((i + 3) % m_hexMapMetrics.hexTileOffsets.size()))
				{
					centerL = pos + m_hexMapMetrics.hexTileOffsets[nnext] * 0.25f * solidFactor;
					centerR = pos + m_hexMapMetrics.hexTileOffsets[prev] * 0.25f * solidFactor;

				}
				else if (hexTile.HasRiverThroughEdge(next))
				{
					centerR = pos;
					centerL = VectorLerp(pos, v2, 0.66f);
				}
				else if (hexTile.HasRiverThroughEdge(prev))
				{
					
					centerR = VectorLerp(pos, v1, 0.66f);
					centerL = pos;
				}
				else if (hexTile.HasRiverThroughEdge(nnext))
				{
					centerR = pos;
					centerL = pos + 0.25f*innerToOuter*solidFactor*(m_hexMapMetrics.hexTileOffsets[next] + m_hexMapMetrics.hexTileOffsets[nnext]);
				}
				else
				{
					centerL = pos;
					centerR = pos + 0.25f*innerToOuter*solidFactor*(m_hexMapMetrics.hexTileOffsets[prev] + m_hexMapMetrics.hexTileOffsets[i]);
				}
				glm::vec3 center = VectorLerp(centerL, centerR, 0.5f);
				glm::vec3 m0 = VectorLerp(centerR, v1, 0.5f);
				const glm::vec3 m1 = VectorLerp(centerR, midv1, 0.5f);
				glm::vec3 m2 = VectorLerp(center, midv2, 0.5f);
				const glm::vec3 m3 = VectorLerp(centerL, midv3, 0.5f);
				const glm::vec3 m4 = VectorLerp(centerL, v2, 0.5f);
				m2.z = center.z = midv2.z;
				

				AddQuad(chunkIndex, m0, v1, m1, midv1, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, m1, midv1, m2, midv2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, m2, midv2, m3, midv3, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, m3, midv3, m4, v2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, centerR, m1, center, m2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddQuad(chunkIndex, center, m2, centerL, m3, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
				AddTriangle(chunkIndex, centerR, m0, m1, hexTile.color, hexTile.color, hexTile.color);
				AddTriangle(chunkIndex, centerL, m3, m4, hexTile.color, hexTile.color, hexTile.color);
				if (!hexTile.IsUnderWater())
				{
					float riverElevation = (hexTile.elevation + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
					if (hexTile.inComingRiver == i)
					{
						AddRiverQuad(chunkIndex, riverElevation, centerR, m1, centerL, m3,
							glm::vec2(0.0f, 0.4f), glm::vec2(0.0f, 0.2f), glm::vec2(1.0f, 0.4f), glm::vec2(1.0f, 0.2f));
						AddRiverQuad(chunkIndex, riverElevation, m1, midv1, m3, midv3,
							glm::vec2(0.0f, 0.2f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.2f), glm::vec2(1.0f, 0.0f));
					}
					else
					{
						AddRiverQuad(chunkIndex, riverElevation, centerR, m1, centerL, m3,
							glm::vec2(1.0f, 0.4f), glm::vec2(1.0f, 0.6f), glm::vec2(0.0f, 0.4f), glm::vec2(0.0f, 0.6f));
						AddRiverQuad(chunkIndex, riverElevation, m1, midv1, m3, midv3,
							glm::vec2(1.0f, 0.6f), glm::vec2(1.0f, 0.8f), glm::vec2(0.0f, 0.6f), glm::vec2(0.0f, 0.8f));
					}
				}
				
				
			}
			


		}
		else
		{
			glm::vec3 center = pos;
			unsigned int prev = (i + 5) % m_hexMapMetrics.hexTileOffsets.size();
			if (hexTile.HasRiverThroughEdge(next))
			{

				if (hexTile.HasRiverThroughEdge(prev))
				{
					center += 0.25f*innerToOuter*m_hexMapMetrics.tileMetrics.solidRegionFactor*(m_hexMapMetrics.hexTileOffsets[i] + m_hexMapMetrics.hexTileOffsets[next]);
				}
				else if (hexTile.HasRiverThroughEdge((i + 4) % m_hexMapMetrics.hexTileOffsets.size()))
				{
					center += m_hexMapMetrics.hexTileOffsets[i] * 0.25f * m_hexMapMetrics.tileMetrics.solidRegionFactor;
				}
			}
			else if (hexTile.HasRiverThroughEdge(prev) && hexTile.HasRiverThroughEdge((i + 2) % m_hexMapMetrics.hexTileOffsets.size()))
			{
				center += m_hexMapMetrics.hexTileOffsets[next] * 0.25f * m_hexMapMetrics.tileMetrics.solidRegionFactor;
			}
			const glm::vec3 m0 = VectorLerp(center, v1, 0.5f);
			const glm::vec3 m1 = VectorLerp(center, midv1, 0.5f);
			const glm::vec3 m2 = VectorLerp(center, midv2, 0.5f);
			const glm::vec3 m3 = VectorLerp(center, midv3, 0.5f);
			const glm::vec3 m4 = VectorLerp(center, v2, 0.5f);
			AddQuad(chunkIndex, m0, v1, m1, midv1, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
			AddQuad(chunkIndex, m1, midv1, m2, midv2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
			AddQuad(chunkIndex, m2, midv2, m3, midv3, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
			AddQuad(chunkIndex, m3, midv3, m4, v2, hexTile.color, hexTile.color, hexTile.color, hexTile.color);
			AddTriangle(chunkIndex, center, m0, m1, hexTile.color, hexTile.color, hexTile.color);
			AddTriangle(chunkIndex, center, m1, m2, hexTile.color, hexTile.color, hexTile.color);
			AddTriangle(chunkIndex, center, m2, m3, hexTile.color, hexTile.color, hexTile.color);
			AddTriangle(chunkIndex, center, m3, m4, hexTile.color, hexTile.color, hexTile.color);
			

		}
		
	}
}

void HexMap::AddMainHexTileUnderWater(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile)
{
	glm::vec3 center = pos;
	center.z = (hexTile.waterLevel + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
	for (unsigned int i = 0; i < m_hexMapMetrics.hexTileOffsets.size(); i++)
	{

		int neighborIndex = hexTile.neighbors[i];
		if (neighborIndex != -1 && !m_tiles[neighborIndex].IsUnderWater())
		{
			AddMainHexTileDirectionWaterShore(chunkIndex, center, hexTile,i);
		}
		else
		{
			AddMainHexTileDirectionOpenWater(chunkIndex, center, hexTile, i);
		}

		

	}
}

void HexMap::AddMainHexTileDirectionOpenWater(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile, unsigned int direction)
{
	auto next = (direction + 1) % m_hexMapMetrics.hexTileOffsets.size();
	glm::vec3 v1 = pos + m_hexMapMetrics.hexTileOffsets[direction] * waterSolidFactor;
	glm::vec3 v2 = pos + m_hexMapMetrics.hexTileOffsets[next] * waterSolidFactor;
	AddWaterTriangle(chunkIndex, pos, v1, v2);
	if (direction <= 2)
	{
		if (hexTile.neighbors[direction] == -1)
			return;
		glm::vec3 connectionOffset = (1.0f - waterSolidFactor)*(m_hexMapMetrics.hexTileOffsets[direction] + m_hexMapMetrics.hexTileOffsets[next]);
		AddWaterQuad(chunkIndex, v1, v1 + connectionOffset, v2, v2 + connectionOffset);
		if (direction <= 1)
		{
			int nextNeighborIndex = hexTile.neighbors[next];
			if (nextNeighborIndex == -1)
				return;
			const HexTile &nextNeighbor = m_tiles[nextNeighborIndex];
			if (!nextNeighbor.IsUnderWater())
				return;
			auto nnext = (direction + 2) % m_hexMapMetrics.hexTileOffsets.size();
			glm::vec3 nextConnectionOffset = (1.0f - waterSolidFactor)*(m_hexMapMetrics.hexTileOffsets[next] + m_hexMapMetrics.hexTileOffsets[nnext]);
			AddWaterTriangle(chunkIndex, v2, v2 + connectionOffset, v2 + nextConnectionOffset);
		}
	}
	
}

void HexMap::AddMainHexTileDirectionWaterShore(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &pos, const HexTile &hexTile, unsigned int direction)
{
	auto next = (direction + 1) % m_hexMapMetrics.hexTileOffsets.size();
	const glm::vec3 v1 = pos + m_hexMapMetrics.hexTileOffsets[direction] * waterSolidFactor;
	const glm::vec3 v2 = pos + m_hexMapMetrics.hexTileOffsets[next] * waterSolidFactor;
	const glm::vec3 midv1 = (3.0f / 4.0f) * v1 + (1.0f / 4.0f) * v2;
	const glm::vec3 midv2 = (1.0f / 2.0f) * v1 + (1.0f / 2.0f) * v2;
	const glm::vec3 midv3 = (1.0f / 4.0f) * v1 + (3.0f / 4.0f) * v2;
	AddWaterTriangle(chunkIndex, pos, v1, midv1);
	AddWaterTriangle(chunkIndex, pos, midv1, midv2);
	AddWaterTriangle(chunkIndex, pos, midv2, midv3);
	AddWaterTriangle(chunkIndex, pos, midv3, v2);
	const glm::vec2 frontUV = glm::vec2(0.0f, 1.0f);
	const glm::vec2 backUV = glm::vec2(0.0f, 0.0f);
	glm::vec3 neighborCenter = m_tiles[hexTile.neighbors[direction]].center;
	neighborCenter.z = pos.z;
	const glm::vec3 v3 = neighborCenter + m_hexMapMetrics.hexTileOffsets[(direction + 4) % 6] * solidFactor;
	const glm::vec3 v4 = neighborCenter + m_hexMapMetrics.hexTileOffsets[(direction + 3) % 6] * solidFactor;
	const glm::vec3 m1 = (3.0f / 4.0f) * v3 + (1.0f / 4.0f) * v4;
	const glm::vec3 m2 = (1.0f / 2.0f) * v3 + (1.0f / 2.0f) * v4;
	const glm::vec3 m3 = (1.0f / 4.0f) * v3 + (3.0f / 4.0f) * v4;
	if (hexTile.HasRiverThroughEdge(direction))
	{
		AddMainHexTileDirectionEstuary(chunkIndex, hexTile.inComingRiver == direction, v1, midv1, midv2, midv3, v2, v3, m1, m2, m3, v4);
	}
	else
	{
		AddUVQuad(m_chunkShoreVertices, chunkIndex, v1, v3, midv1, m1, backUV, frontUV, backUV, frontUV);
		AddUVQuad(m_chunkShoreVertices, chunkIndex, midv1, m1, midv2, m2, backUV, frontUV, backUV, frontUV);
		AddUVQuad(m_chunkShoreVertices, chunkIndex, midv2, m2, midv3, m3, backUV, frontUV, backUV, frontUV);
		AddUVQuad(m_chunkShoreVertices, chunkIndex, midv3, m3, v2, v4, backUV, frontUV, backUV, frontUV);
	}
		int nextNeighborIndex = hexTile.neighbors[next];
		if (nextNeighborIndex == -1)
			return;
		const HexTile &nextNeighbor = m_tiles[nextNeighborIndex];
		glm::vec3 v5 = nextNeighbor.center
			+ m_hexMapMetrics.hexTileOffsets[(direction + 5) % 6] * (nextNeighbor.IsUnderWater() ? waterSolidFactor : solidFactor);
		v5.z = pos.z;
		AddUVTriangle(m_chunkShoreVertices, chunkIndex,v2, v4, v5, backUV, frontUV, nextNeighbor.IsUnderWater() ? backUV : frontUV);
	
	
}



void HexMap::AddMainHexTileDirectionEstuary(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex,bool incomingRiver, const glm::vec3 &e0, const glm::vec3 &e1, const glm::vec3 &e2, const glm::vec3 &e3, const glm::vec3 &e4, const glm::vec3 &e5, const glm::vec3 &e6, const glm::vec3 &e7, const glm::vec3 &e8, const glm::vec3 &e9)
{
	const glm::vec2 frontUV = glm::vec2(0.0f, 1.0f);
	const glm::vec2 backUV = glm::vec2(0.0f, 0.0f);
	const glm::vec2 frontinnerUV = glm::vec2(1.0f, 1.0f);
	AddUVTriangle(m_chunkShoreVertices, chunkIndex, e3, e9, e4, backUV, frontUV, backUV);
	AddUVTriangle(m_chunkShoreVertices, chunkIndex,e0, e5, e1, backUV, frontUV, backUV);
	if (incomingRiver)
	{
		
		AddUVUVTriangle(m_chunkEstuaryVertices, chunkIndex, e2, e6, e8, backUV, frontinnerUV, frontinnerUV, glm::vec2(0.5f, 1.1f), glm::vec2(0.0f, 0.8f), glm::vec2(1.0f, 0.8f));
	
		AddUVUVQuad(m_chunkEstuaryVertices, chunkIndex, e6, e2, e5, e1, frontinnerUV, backUV, frontUV, backUV, glm::vec2(0.0f, 0.8f), glm::vec2(0.5f, 1.1f), glm::vec2(-0.5f, 1.0f), glm::vec2(0.3f, 1.15f));
		AddUVUVQuad(m_chunkEstuaryVertices, chunkIndex, e8, e9, e2, e3, frontinnerUV, frontUV, backUV, backUV, glm::vec2(1.0f, 0.8f), glm::vec2(1.5f, 1.0f), glm::vec2(0.5f, 1.1f), glm::vec2(0.7f, 1.15f));
	}
	else
	{
			AddUVUVTriangle(m_chunkEstuaryVertices, chunkIndex, /*m_chunkEstuaryTriangles,*/ e2, e6, e8, backUV, frontinnerUV, frontinnerUV, glm::vec2(0.5f, -0.3f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
			AddUVUVQuad(m_chunkEstuaryVertices, chunkIndex,/* m_chunkEstuaryTriangles,*/ e6, e2, e5, e1, frontinnerUV, backUV, frontUV, backUV, glm::vec2(1.0f, 0.0f), glm::vec2(0.5f, -0.3f), glm::vec2(1.5f, -0.2f), glm::vec2(0.7f, -0.35f));
			AddUVUVQuad(m_chunkEstuaryVertices, chunkIndex,/* m_chunkEstuaryTriangles,*/ e8, e9, e2, e3, frontinnerUV, frontUV, backUV, backUV, glm::vec2(0.0f, 0.0f), glm::vec2(-0.5f, -0.2f), glm::vec2(0.5f, -0.3f), glm::vec2(0.3f, -0.35f));

	}
	
}

void HexMap::AddGradeTwoConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const HexTile &hextile, const glm::vec3& center, unsigned int direction)
{
	int neighborIndex = hextile.neighbors[direction];
	if (neighborIndex == -1)
		return;


	const HexTile &neighborTile = m_tiles[neighborIndex];
	//unsigned int currentIndex = (unsigned int)vertices.size();
	glm::vec4 color = hextile.color;
	glm::vec4 neighborColor = neighborTile.color;
	glm::vec3 connectionOffset = (1.0f - m_hexMapMetrics.tileMetrics.solidRegionFactor)*(m_hexMapMetrics.hexTileOffsets[direction] + m_hexMapMetrics.hexTileOffsets[(direction + 1) % 6]);
	float elevation = neighborTile.center.z;

	glm::vec3 v0 = center + m_hexMapMetrics.tileMetrics.solidRegionFactor * m_hexMapMetrics.hexTileOffsets[direction];
	glm::vec3 v1 = v0 + connectionOffset;
	v1.z = elevation;
	glm::vec3 v2 = center + m_hexMapMetrics.tileMetrics.solidRegionFactor * m_hexMapMetrics.hexTileOffsets[(direction + 1) % 6];
	glm::vec3 v3 = v2 + connectionOffset;
	v3.z= elevation;
	
	bool hasRiverThroughEdge = hextile.HasRiverThroughEdge(direction);
	float newz =  hasRiverThroughEdge ? (hextile.elevation + m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep : 0.0f;
	float newNeightborz = hasRiverThroughEdge ? (neighborTile.elevation + m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep : 0.0f;
	

	HexTile::EdgeType edgetype = hextile.getEdgeType(neighborTile);
	switch (edgetype)
	{
	case HexTile::EdgeType::Slope:
	{
		for (unsigned int step = 0; step < m_hexMapMetrics.terraceSteps; step++)
		{
			//unsigned int currentIndex = (unsigned int)vertices.size();
			glm::vec4 c0 = m_hexMapMetrics.ColorTerraceLerp(color, neighborColor, step);
			glm::vec4 c1 = m_hexMapMetrics.ColorTerraceLerp(color, neighborColor, step + 1);
			glm::vec3 prev = m_hexMapMetrics.TerraceLerp(v0, v1, step);
			glm::vec3 next = m_hexMapMetrics.TerraceLerp(v0, v1, step + 1);
			glm::vec3 prev1 = m_hexMapMetrics.TerraceLerp(v2, v3, step);
			glm::vec3 next1 = m_hexMapMetrics.TerraceLerp(v2, v3, step + 1);
			glm::vec3 e0 = (3.0f / 4.0f) * prev + (1.0f / 4.0f) * prev1;
			
			glm::vec3 e4 = (1.0f / 4.0f) * prev + (3.0f / 4.0f) * prev1;
			glm::vec3 e1 = (3.0f / 4.0f) * next + (1.0f / 4.0f) * next1;
			
			glm::vec3 e5 = (1.0f / 4.0f) * next + (3.0f / 4.0f) * next1;

			glm::vec3 e2, e3;
			if (hasRiverThroughEdge)
			{
				glm::vec3 prev = m_hexMapMetrics.TerraceLerp(glm::vec3(v0.x,v0.y,newz), glm::vec3(v1.x, v1.y, newNeightborz), step);
				glm::vec3 next = m_hexMapMetrics.TerraceLerp(glm::vec3(v0.x, v0.y, newz), glm::vec3(v1.x, v1.y, newNeightborz), step + 1);
				glm::vec3 prev1 = m_hexMapMetrics.TerraceLerp(glm::vec3(v2.x, v2.y, newz), glm::vec3(v3.x, v3.y, newNeightborz), step);
				glm::vec3 next1 = m_hexMapMetrics.TerraceLerp(glm::vec3(v2.x, v2.y, newz), glm::vec3(v3.x, v3.y, newNeightborz), step + 1);
				 e2 = (1.0f / 2.0f) * prev + (1.0f / 2.0f) * prev1;
				 e3 = (1.0f / 2.0f) * next + (1.0f / 2.0f) * next1;
				 

			}
			else
			{
				 e2 = (1.0f / 2.0f) * prev + (1.0f / 2.0f) * prev1;
				 e3 = (1.0f / 2.0f) * next + (1.0f / 2.0f) * next1;
			}
			AddQuad(chunkIndex, prev, next, e0, e1, c0, c1, c0, c1);
			AddQuad(chunkIndex, e0, e1, e2, e3, c0, c1, c0, c1);
			AddQuad(chunkIndex, e2, e3, e4, e5, c0, c1, c0, c1);
			AddQuad(chunkIndex, e4, e5, prev1, next1, c0, c1, c0, c1);


		}
		if (hasRiverThroughEdge && !hextile.IsUnderWater() && !neighborTile.IsUnderWater())
		{
			glm::vec3 e0 = (3.0f / 4.0f) * v0 + (1.0f / 4.0f) * v2;
			glm::vec3 e4 = (1.0f / 4.0f) * v0 + (3.0f / 4.0f) * v2;
			glm::vec3 e1 = (3.0f / 4.0f) * v1 + (1.0f / 4.0f) * v3;
			glm::vec3 e5 = (1.0f / 4.0f) * v1 + (3.0f / 4.0f) * v3;
			float elevation = (hextile.elevation + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
			float neightborElevation = (neighborTile.elevation + 0.25f * m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
			if (hextile.HasIncomingRiver() && hextile.inComingRiver == direction)
				AddRiverQuad(chunkIndex, elevation, neightborElevation, e0, e1, e4, e5,
					glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, -0.2f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, -0.2f));
			else
				AddRiverQuad(chunkIndex, elevation, neightborElevation, e0, e1, e4, e5,
					glm::vec2(1.0f, 0.8f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.8f), glm::vec2(0.0f, 1.0f));
		}
		
		break;
	}
	case HexTile::EdgeType::Flat:
	case HexTile::EdgeType::Cliff:
	{
		glm::vec3 e0 = (3.0f / 4.0f) * v0 + (1.0f / 4.0f) * v2;
		glm::vec3 e2 = (1.0f / 2.0f) * v0 + (1.0f / 2.0f) * v2;
		glm::vec3 e4 = (1.0f / 4.0f) * v0 + (3.0f / 4.0f) * v2;
		glm::vec3 e1 = (3.0f / 4.0f) * v1 + (1.0f / 4.0f) * v3;
		glm::vec3 e3 = (1.0f / 2.0f) * v1 + (1.0f / 2.0f) * v3;
		glm::vec3 e5 = (1.0f / 4.0f) * v1 + (3.0f / 4.0f) * v3;
		if (hasRiverThroughEdge)
		{
			e2.z = newz;
			e3.z = newNeightborz;
			float elevation = (hextile.elevation + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
			float neightborElevation = (m_tiles[neighborIndex].elevation + 0.25f * m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep;
			if(!hextile.IsUnderWater())
			{
				if (!neighborTile.IsUnderWater())
				{
					if (hextile.HasIncomingRiver() && hextile.inComingRiver == direction)
						AddRiverQuad(chunkIndex, elevation, neightborElevation, e0, e1, e4, e5,
							glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, -0.2f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, -0.2f));
					else
						AddRiverQuad(chunkIndex, elevation, neightborElevation, e0, e1, e4, e5,
							glm::vec2(1.0f, 0.8f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.8f), glm::vec2(0.0f, 1.0f));
				}
				else if (hextile.elevation > neighborTile.elevation)
				{
					AddWaterfall(chunkIndex, e0,e1,e4,e5, elevation, neightborElevation, (neighborTile.waterLevel + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep,
						glm::vec2(1.0f, 0.8f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.8f), glm::vec2(0.0f, 1.0f));
			
				}
				
			}
			else if (!neighborTile.IsUnderWater() && neighborTile.elevation > hextile.elevation)
			{
				AddWaterfall(chunkIndex, e5, e4, e1, e0, neightborElevation, elevation, (hextile.waterLevel + 0.25f*m_hexMapMetrics.tileMetrics.waterElevationOffset) * m_hexMapMetrics.tileMetrics.elevationStep,
					glm::vec2(1.0f, 0.8f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.8f), glm::vec2(0.0f, 1.0f));
			
			
			}
		}
		AddQuad(chunkIndex, v0, v1, e0, e1, color, neighborColor, color, neighborColor);
		AddQuad(chunkIndex, e0, e1, e2, e3, color, neighborColor, color, neighborColor);
		AddQuad(chunkIndex, e2, e3, e4, e5, color, neighborColor, color, neighborColor);
		AddQuad(chunkIndex, e4, e5, v2, v3, color, neighborColor, color, neighborColor);
		break;
	}

	}
}

void HexMap::AddGradeThreeConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const HexTile &hextile, const glm::vec3& center, unsigned int direction)
{
	int neighborIndex = hextile.neighbors[direction];
	int prevNeighborIndex = hextile.neighbors[(direction + 5) % 6];
	if (neighborIndex == -1 || prevNeighborIndex == -1)
		return;

	const HexTile &neighborTile = m_tiles[neighborIndex];
	const HexTile &prevneighborTile = m_tiles[prevNeighborIndex];

	glm::vec3 v0 = center + m_hexMapMetrics.tileMetrics.solidRegionFactor * m_hexMapMetrics.hexTileOffsets[direction];
	glm::vec3 prevNeighborOffset = (1.0f - m_hexMapMetrics.tileMetrics.solidRegionFactor)*(m_hexMapMetrics.hexTileOffsets[direction] + m_hexMapMetrics.hexTileOffsets[(direction + 5) % 6]);
	glm::vec3 neighborOffset = (1.0f - m_hexMapMetrics.tileMetrics.solidRegionFactor)*(m_hexMapMetrics.hexTileOffsets[direction] + m_hexMapMetrics.hexTileOffsets[(direction + 1) % 6]);

	glm::vec3 v1 = v0 + prevNeighborOffset;
	v1.z = prevneighborTile.center.z ;
	glm::vec3 v2 = v0 + neighborOffset;
	v2.z = neighborTile.center.z;


	if (hextile.elevation <= neighborTile.elevation)
	{
		if (hextile.elevation <= prevneighborTile.elevation)
			AddOrdenedGradeThreeConnection(chunkIndex,v0, hextile, v1, prevneighborTile, v2, neighborTile);
		else
			AddOrdenedGradeThreeConnection(chunkIndex,v1, prevneighborTile, v2, neighborTile, v0, hextile);
	}
	else if (prevneighborTile.elevation <= neighborTile.elevation)
		AddOrdenedGradeThreeConnection(chunkIndex,v1, prevneighborTile, v2, neighborTile, v0, hextile);
	else
		AddOrdenedGradeThreeConnection(chunkIndex,v2, neighborTile, v0, hextile, v1, prevneighborTile);
}

void HexMap::AddOrdenedGradeThreeConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &bottom, const HexTile &bottomTile, const glm::vec3 &right, const HexTile &rightTile, const glm::vec3 &left, const HexTile &leftTile)
{
	HexTile::EdgeType rightEdgeType = bottomTile.getEdgeType(rightTile);
	HexTile::EdgeType leftTEdgeType = bottomTile.getEdgeType(leftTile);

	if (leftTEdgeType == HexTile::EdgeType::Slope)
	{
		if (rightEdgeType == HexTile::EdgeType::Slope)
			AddSSFConnection(chunkIndex, bottom, bottomTile, right, rightTile, left, leftTile);

		else if (rightEdgeType == HexTile::EdgeType::Flat)
			AddSSFConnection(chunkIndex, left, leftTile, bottom, bottomTile, right, rightTile);
		else
			AddSCConnection(chunkIndex, bottom, bottomTile, right, rightTile, left, leftTile);
	}
	else if (rightEdgeType == HexTile::EdgeType::Slope)
	{
		if (leftTEdgeType == HexTile::EdgeType::Flat)
			AddSSFConnection(chunkIndex, right, rightTile, left, leftTile, bottom, bottomTile);
		else
			AddCSConnection(chunkIndex, bottom, bottomTile, right, rightTile, left, leftTile);

	}
	else if (rightTile.getEdgeType(leftTile) == HexTile::EdgeType::Slope)
	{
		if (leftTile.elevation < rightTile.elevation)
			AddCSConnection(chunkIndex, right, rightTile, left, leftTile, bottom, bottomTile);
		else
			AddSCConnection(chunkIndex, left, leftTile, bottom, bottomTile, right, rightTile);
	}
	else
		AddTriangle(chunkIndex, bottom, right, left, bottomTile.color, rightTile.color, leftTile.color);
}

void HexMap::AddSSFConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile, const glm::vec3 &right, const HexTile &rightTile, const glm::vec3 &left, const HexTile &leftTile)
{
	glm::vec3 v3 = m_hexMapMetrics.TerraceLerp(begin, left, 1);
	glm::vec3 v4 = m_hexMapMetrics.TerraceLerp(begin, right, 1);
	glm::vec4 c3 = m_hexMapMetrics.ColorTerraceLerp(beginTile.color, leftTile.color, 1);
	glm::vec4 c4 = m_hexMapMetrics.ColorTerraceLerp(beginTile.color, rightTile.color, 1);

	AddTriangle(chunkIndex, begin, v4, v3, beginTile.color, c4, c3);

	for (unsigned int step = 2; step < m_hexMapMetrics.terraceSteps; step++)
	{
		glm::vec3 v1 = v3;
		glm::vec3 v2 = v4;
		glm::vec4 c1 = c3;
		glm::vec4 c2 = c4;
		v3 = m_hexMapMetrics.TerraceLerp(begin, left, step);
		v4 = m_hexMapMetrics.TerraceLerp(begin, right, step);
		c3 = m_hexMapMetrics.ColorTerraceLerp(beginTile.color, leftTile.color, step);
		c4 = m_hexMapMetrics.ColorTerraceLerp(beginTile.color, rightTile.color, step);

		AddQuad(chunkIndex, v2, v4, v1, v3, c2, c4, c1, c3);

	}
	AddQuad(chunkIndex, v4, right, v3, left, c4, rightTile.color, c3, leftTile.color);
}

void HexMap::AddSCConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile, const glm::vec3 &right, const HexTile &rightTile, const glm::vec3 &left, const HexTile &leftTile)
{
	float b = (float)(leftTile.elevation - beginTile.elevation) / (rightTile.elevation - beginTile.elevation);
	glm::vec3 boundary = (1-b) * (begin + SampleNoise3D(m_hexMapMetrics.noise,begin)) + b*(right + SampleNoise3D(m_hexMapMetrics.noise, right));
	glm::vec4 colorBoundary = beginTile.color + b*(rightTile.color - beginTile.color);
	AddSCBoundary(chunkIndex, begin, beginTile, left, leftTile, boundary, colorBoundary);

	if (leftTile.getEdgeType(rightTile) == HexTile::EdgeType::Cliff)
		AddTriangleWithouthNoise(chunkIndex, left + SampleNoise3D(m_hexMapMetrics.noise,left), boundary, right + SampleNoise3D(m_hexMapMetrics.noise, right), leftTile.color, colorBoundary, rightTile.color);
	else
		AddSCBoundary(chunkIndex, left, leftTile, right, rightTile, boundary, colorBoundary);
}

void HexMap::AddCSConnection(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile, const glm::vec3 &right, const HexTile &rightTile, const glm::vec3 &left, const HexTile &leftTile)
{
	float b = (float)(rightTile.elevation - beginTile.elevation) / (leftTile.elevation - beginTile.elevation);
	glm::vec3 boundary = (1-b)* (begin + SampleNoise3D(m_hexMapMetrics.noise, begin)) + b*(left + SampleNoise3D(m_hexMapMetrics.noise, left));
	glm::vec4 colorBoundary = beginTile.color + b*(leftTile.color - beginTile.color);
	AddSCBoundary(chunkIndex, right, rightTile, begin, beginTile, boundary, colorBoundary);

	if (rightTile.getEdgeType(leftTile) == HexTile::EdgeType::Cliff)
		AddTriangleWithouthNoise(chunkIndex, boundary, right + SampleNoise3D(m_hexMapMetrics.noise, right), left  + SampleNoise3D(m_hexMapMetrics.noise, left), colorBoundary, rightTile.color, leftTile.color);
	else
		AddSCBoundary(chunkIndex, left, leftTile, right, rightTile, boundary, colorBoundary);
}

void HexMap::AddSCBoundary(std::vector<std::vector<PositionNormalColorVertexData>>::size_type chunkIndex, const glm::vec3 &begin, const HexTile &beginTile, const glm::vec3 &left, const HexTile &leftTile, const glm::vec3 &boundary, const glm::vec4 &boundaryColor)
{
	glm::vec3 v0 = begin;
	glm::vec4 c0 = beginTile.color;
	for (unsigned int i = 1; i <= m_hexMapMetrics.terraceSteps; i++)
	{
		glm::vec3 v2 = m_hexMapMetrics.TerraceLerp(begin, left, i);
		glm::vec4 c2 = m_hexMapMetrics.ColorTerraceLerp(beginTile.color, leftTile.color, i);

		AddTriangleWithouthNoise(chunkIndex, v0 + SampleNoise3D(m_hexMapMetrics.noise, v0), boundary, v2 + SampleNoise3D(m_hexMapMetrics.noise, v2), c0, boundaryColor, c2);
		v0 = v2;
		c0 = c2;
	}
}







void HexMap::InitializeCPURenderData()
{
	
	m_chunkVertices.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_chunkRiverVertices.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_chunkWaterVertices.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_chunkShoreVertices.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_chunkEstuaryVertices.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);

	for (unsigned int i = 0; i < m_hexMapMetrics.width*m_hexMapMetrics.height; i++)
	{
		m_chunkVertices.push_back(std::vector<PositionNormalColorVertexData>());
		m_chunkVertices[i].reserve(MAX_VERTICES_PER_TILE*CHUNK_SIZE*CHUNK_SIZE);
	
		m_chunkRiverVertices.push_back(std::vector<PositionUVVertexData>());
		m_chunkRiverVertices[i].reserve(MAX_RIVER_VERTICES_PER_TILE*CHUNK_SIZE*CHUNK_SIZE);
		
	
		m_chunkWaterVertices.push_back(std::vector<PositionVertexData>());
		m_chunkWaterVertices[i].reserve(MAX_WATER_VERTICES_PER_TILE*CHUNK_SIZE*CHUNK_SIZE);


		m_chunkShoreVertices.push_back(std::vector<PositionUVVertexData>());
		m_chunkShoreVertices[i].reserve(MAX_SHORE_VERTICES_PER_TILE*CHUNK_SIZE*CHUNK_SIZE);

		m_chunkEstuaryVertices.push_back(std::vector<PositionUVUVVertexData>());
		m_chunkEstuaryVertices[i].reserve(MAX_ESTUARY_VERTICES_PER_TILE*CHUNK_SIZE*CHUNK_SIZE);

	}


}

void HexMap::InitializeGPURenderData()
{
	m_vao.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_vbo.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);

	
	m_riverVao.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_riverVbo.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);

	m_waterVao.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_waterVbo.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);


	m_shoreVao.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_shoreVbo.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);


	m_estuaryVao.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	m_estuaryVbo.reserve(m_hexMapMetrics.width*m_hexMapMetrics.height);
	
	Nova::BufferLayout positionNormalColor =
	{
		{ Nova::ShaderDataType::Float3 , "a_Position", 0 },
		{ Nova::ShaderDataType::Float3 , "a_Normal", 1 },
		{ Nova::ShaderDataType::Float4 , "a_Color", 2 }
	};
	Nova::BufferLayout positionUVLayout =
	{
		{ Nova::ShaderDataType::Float3 , "a_Position", 0},
		{ Nova::ShaderDataType::Float2 , "a_TexCoord", 1}
	};

	Nova::BufferLayout positionLayout =
	{
		{Nova::ShaderDataType::Float3 , "a_Position", 0}
	};

	Nova::BufferLayout positionUVUVLayout = 
	{
		{ Nova::ShaderDataType::Float3, "a_Position", 0},
		{ Nova::ShaderDataType::Float2, "a_TexCoord0", 1},
		{ Nova::ShaderDataType::Float2, "a_TexCoord1", 2}
	};
	for (unsigned int i = 0; i < m_hexMapMetrics.width*m_hexMapMetrics.height; i++)
	{
		m_vbo.push_back(Nova::VertexBuffer::Create(nullptr, CHUNK_SIZE*CHUNK_SIZE*MAX_VERTICES_PER_TILE*(3 + 3 + 4) * sizeof(float), positionNormalColor, Nova::BufferType::DynamicDraw));
		m_vao.push_back(Nova::VertexArray::Create());
		m_vao[i]->AddVertexBuffer(*m_vbo[i]);

		m_riverVbo.push_back(Nova::VertexBuffer::Create(nullptr, CHUNK_SIZE*CHUNK_SIZE*MAX_RIVER_VERTICES_PER_TILE*(3 + 2) * sizeof(float), positionUVLayout, Nova::BufferType::DynamicDraw));
		m_riverVao.push_back(Nova::VertexArray::Create());
		m_riverVao[i]->AddVertexBuffer(*m_riverVbo[i]);

		m_waterVbo.push_back(Nova::VertexBuffer::Create(nullptr, CHUNK_SIZE*CHUNK_SIZE*MAX_WATER_VERTICES_PER_TILE*(3) * sizeof(float), positionLayout, Nova::BufferType::DynamicDraw));
		m_waterVao.push_back(Nova::VertexArray::Create());
		m_waterVao[i]->AddVertexBuffer(*m_waterVbo[i]);
		
		m_shoreVbo.push_back(Nova::VertexBuffer::Create(nullptr, CHUNK_SIZE*CHUNK_SIZE*MAX_SHORE_VERTICES_PER_TILE*(3 + 2) * sizeof(float), positionUVLayout, Nova::BufferType::DynamicDraw));
		m_shoreVao.push_back(Nova::VertexArray::Create());
		m_shoreVao[i]->AddVertexBuffer(*m_shoreVbo[i]);


		m_estuaryVbo.push_back(Nova::VertexBuffer::Create(nullptr, CHUNK_SIZE*CHUNK_SIZE*MAX_ESTUARY_VERTICES_PER_TILE*(3 + 2 + 2) * sizeof(float), positionUVUVLayout, Nova::BufferType::DynamicDraw));
		m_estuaryVao.push_back(Nova::VertexArray::Create());
		m_estuaryVao[i]->AddVertexBuffer(*m_estuaryVbo[i]);
	}

	
}

void HexMap::UpdateCPURenderData()
{

	m_chunkAABBs = std::vector<Nova::AABB>(m_hexMapMetrics.width*m_hexMapMetrics.height, Nova::AABB(glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::min())));
	for (auto &hexTile : m_tiles)
	{
		unsigned int vcIndex = (unsigned int)m_chunkVertices[hexTile.chunkIndex].size();
		hexTile.firstVertexIndex = vcIndex;

		AddMainHexTile(hexTile.chunkIndex, hexTile.center, hexTile);
		AddGradeTwoConnection(hexTile.chunkIndex, hexTile, hexTile.center, 5);
		AddGradeTwoConnection(hexTile.chunkIndex, hexTile, hexTile.center, 4);
		AddGradeTwoConnection(hexTile.chunkIndex, hexTile, hexTile.center, 3);

		AddGradeThreeConnection(hexTile.chunkIndex, hexTile, hexTile.center, 5);
		AddGradeThreeConnection(hexTile.chunkIndex, hexTile, hexTile.center, 4);

		hexTile.vertexCount = (unsigned int)m_chunkVertices[hexTile.chunkIndex].size() - vcIndex;

	}
}
void HexMap::UpdateCPURenderData(const std::unordered_set<unsigned int> &chunks)
{
	for (auto chunk : chunks)
	{
		m_chunkVertices[chunk].clear();
		m_chunkRiverVertices[chunk].clear();
		m_chunkWaterVertices[chunk].clear();
		m_chunkShoreVertices[chunk].clear();
		m_chunkEstuaryVertices[chunk].clear();
		m_chunkAABBs[chunk] = Nova::AABB(glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::min()));
	}
	
	

		for (auto &hexTile : m_tiles)
		{
			if (chunks.find(hexTile.chunkIndex) != chunks.end())
			{
				unsigned int vcIndex = (unsigned int)m_chunkVertices[hexTile.chunkIndex].size();


				hexTile.firstVertexIndex = vcIndex;

				AddMainHexTile(hexTile.chunkIndex, hexTile.center, hexTile);

				AddGradeTwoConnection(hexTile.chunkIndex, hexTile, hexTile.center, 5);
				AddGradeTwoConnection(hexTile.chunkIndex, hexTile, hexTile.center, 4);
				AddGradeTwoConnection(hexTile.chunkIndex, hexTile, hexTile.center, 3);

				AddGradeThreeConnection(hexTile.chunkIndex, hexTile, hexTile.center, 5);
				AddGradeThreeConnection(hexTile.chunkIndex, hexTile, hexTile.center, 4);

				hexTile.vertexCount = (unsigned int)m_chunkVertices[hexTile.chunkIndex].size() - vcIndex;

			}


		}

}

void HexMap::UpdateGPURenderData()
{
	for (unsigned int i = 0; i < m_hexMapMetrics.width*m_hexMapMetrics.height; i++)
	{

		m_vbo[i]->UpdateSubData(m_chunkVertices[i].size() > 0 ? &m_chunkVertices[i].front() : nullptr, (unsigned int)m_chunkVertices[i].size() * (3 + 3 + 4) * sizeof(float), 0);
		m_riverVbo[i]->UpdateSubData(m_chunkRiverVertices[i].size() > 0 ? &m_chunkRiverVertices[i].front() : nullptr, (unsigned int)m_chunkRiverVertices[i].size() * (3 + 2) * sizeof(float), 0);
		m_waterVbo[i]->UpdateSubData(m_chunkWaterVertices[i].size() > 0 ? &m_chunkWaterVertices[i].front() : nullptr, (unsigned int)m_chunkWaterVertices[i].size() * (3) * sizeof(float), 0);
		m_shoreVbo[i]->UpdateSubData(m_chunkShoreVertices[i].size() > 0 ? &m_chunkShoreVertices[i].front() : nullptr, (unsigned int)m_chunkShoreVertices[i].size() * (3 + 2) * sizeof(float), 0);
		m_estuaryVbo[i]->UpdateSubData(m_chunkEstuaryVertices[i].size() > 0 ? &m_chunkEstuaryVertices[i].front() : nullptr, (unsigned int)m_chunkEstuaryVertices[i].size() * (3 + 2 + 2) * sizeof(float), 0);
		m_vao[i]->SetVertexCount((unsigned int)m_chunkVertices[i].size());
		m_riverVao[i]->SetVertexCount((unsigned int)m_chunkRiverVertices[i].size());
		m_waterVao[i]->SetVertexCount((unsigned int)m_chunkWaterVertices[i].size());
		m_shoreVao[i]->SetVertexCount((unsigned int)m_chunkShoreVertices[i].size());
		m_estuaryVao[i]->SetVertexCount((unsigned int)m_chunkEstuaryVertices[i].size());
	}
}

void HexMap::UpdateGPURenderData(const std::unordered_set<unsigned int> &chunks)
{
	
	for (const auto chunk : chunks)
	{
		m_vbo[chunk]->UpdateSubData(m_chunkVertices[chunk].size() > 0 ? &m_chunkVertices[chunk].front() : nullptr, (unsigned int)m_chunkVertices[chunk].size() * (3 + 3 + 4) * sizeof(float), 0);
		m_riverVbo[chunk]->UpdateSubData(m_chunkRiverVertices[chunk].size() > 0 ? &m_chunkRiverVertices[chunk].front() : nullptr, (unsigned int)m_chunkRiverVertices[chunk].size() * (3 + 2) * sizeof(float), 0);
		m_waterVbo[chunk]->UpdateSubData(m_chunkWaterVertices[chunk].size() > 0 ? &m_chunkWaterVertices[chunk].front() : nullptr, (unsigned int)m_chunkWaterVertices[chunk].size() * (3) * sizeof(float), 0);
		m_shoreVbo[chunk]->UpdateSubData(m_chunkShoreVertices[chunk].size() > 0 ? &m_chunkShoreVertices[chunk].front() : nullptr, (unsigned int)m_chunkShoreVertices[chunk].size() * (3 + 2) * sizeof(float), 0);
		m_estuaryVbo[chunk]->UpdateSubData(m_chunkEstuaryVertices[chunk].size() > 0 ? &m_chunkEstuaryVertices[chunk].front() : nullptr, (unsigned int)m_chunkEstuaryVertices[chunk].size() * (3 + 2 + 2) * sizeof(float), 0);
		m_vao[chunk]->SetVertexCount((unsigned int)m_chunkVertices[chunk].size());
		m_riverVao[chunk]->SetVertexCount((unsigned int)m_chunkRiverVertices[chunk].size());
		m_waterVao[chunk]->SetVertexCount((unsigned int)m_chunkWaterVertices[chunk].size());
		m_shoreVao[chunk]->SetVertexCount((unsigned int)m_chunkShoreVertices[chunk].size());
		m_estuaryVao[chunk]->SetVertexCount((unsigned int)m_chunkEstuaryVertices[chunk].size());
	}

			


}
