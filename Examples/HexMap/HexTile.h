#pragma once
#ifndef HEXTILE_H
#define HEXTILE_H
#include <vector>
constexpr size_t featureCount = 2;
constexpr uint8_t outGoingRiverBit = 0;
constexpr uint8_t inComingRiverBit = 1;
constexpr float outerToInner = 0.866025404f;
constexpr float innerToOuter = 1.0f / outerToInner;



struct HexTile
{
public:
	enum class EdgeType
	{
		Flat,
		Slope,
		Cliff
	};
	unsigned int chunkIndex;
	unsigned int firstVertexIndex;
	unsigned int vertexCount;
	glm::vec3 center;
	glm::vec4 color;
	int neighbors[6];
	int elevation;
	int waterLevel;
	int HexCoord[3];

	std::bitset<featureCount> featuresMask;
	unsigned int outGoingRiver;
	unsigned int inComingRiver;

	inline HexTile(glm::vec3 p, int x, int y, unsigned int chunk, unsigned int  e, glm::vec4 c)
		: center(p), chunkIndex(chunk), elevation(e), color(c),
		featuresMask(std::bitset<featureCount>()), inComingRiver(0), outGoingRiver(0),
		vertexCount(0), firstVertexIndex(0), waterLevel(0)
	{

		HexCoord[0] = x - y / 2;
		HexCoord[1] = y;
		HexCoord[2] = -HexCoord[0] - HexCoord[1];

		//Information for this members is not known at construction time.
		for (int i = 0; i < 6; i++)
			neighbors[i] = -1;
	}

	inline EdgeType getEdgeType(const HexTile &neighbor) const
	{
		if (elevation == neighbor.elevation)
			return EdgeType::Flat;
		else if ((neighbor.elevation - elevation) <= 1 && (elevation - neighbor.elevation) <= 1)
			return EdgeType::Slope;
		else
			return EdgeType::Cliff;
	}
	inline bool HasIncomingRiver() const
	{
		return featuresMask[inComingRiverBit];
	}
	inline bool HasOutGoingRiver() const
	{
		return featuresMask[outGoingRiverBit];
	}
	inline bool HasRiver() const
	{
		return featuresMask[outGoingRiverBit] || featuresMask[inComingRiverBit];
	}
	inline bool HasRiverBeginOrEnd() const
	{
		return featuresMask[outGoingRiverBit] != featuresMask[inComingRiverBit];
	}
	inline bool HasRiverThroughEdge(unsigned int direction) const
	{
		return (featuresMask[inComingRiverBit] && inComingRiver == direction) ||
			(featuresMask[outGoingRiverBit] && outGoingRiver == direction);
	}
	inline bool IsUnderWater() const
	{

		return waterLevel > elevation;
	}



};
inline bool IsValidRiverDestination(const HexTile &sourceTile, const HexTile &destinationTile)
{
	return (sourceTile.elevation >= destinationTile.elevation) || (sourceTile.waterLevel == destinationTile.elevation);
}
inline bool AreNeighbors(std::vector<HexTile>::size_type firstIndex, const HexTile &firstTile, std::vector<HexTile>::size_type secondIndex, const HexTile &secondTile, unsigned int &direction)
{
	if (firstIndex == secondIndex)
		return false;
	for (unsigned int i = 0; i < 6; i++)
	{
		if (firstTile.neighbors[i] == secondIndex)
		{
			direction = (i + 3) % 6;
			return true;
		}

	}
	return false;
}


struct HexTileMetrics
{
	float outerRadius;
	float innerRadius;
	float solidRegionFactor;
	float elevationStep;
	float waterElevationOffset;
	HexTileMetrics(float r, float factor, float elevation, float riverElevation)
		: outerRadius(r), innerRadius(outerRadius*outerToInner), solidRegionFactor(factor), elevationStep(elevation), waterElevationOffset(riverElevation) {}

};

#endif