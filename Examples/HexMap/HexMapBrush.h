#pragma once
#ifndef HEXMAPBRUSH_H
#define HEXMAPBRUSH_H
#include "HexMap.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <initializer_list>
#include <Nova/Renderer/Camera.h>

class HexMap;
struct HexTile;
struct BrushColor
{
	glm::vec4 color;
	std::string colorName;

	BrushColor(const glm::vec4 &c, const std::string &name) : color(c), colorName(name) {}
};
enum class RiverEditingMode : uint8_t
{
	Removing,
	Adding

};

inline static RiverEditingMode IntToRiverEditingMode(int i)
{
	if (i == 0)
		return RiverEditingMode::Removing;
	else if (i == 1)
		return RiverEditingMode::Adding;
	else
		return RiverEditingMode::Removing;

}



class HexMapBrush
{
public:
	HexMapBrush() : m_currentBrushSize(0), m_colorPalette(), m_currentColorIndex(0), m_colorEditing(true),
		m_currentElevation(0), m_maxElevation(0), m_elevationEditing(false),
		m_currentTileIndex(0), m_prevTileIndex(0), m_riverEditing(false), m_riverEditingMode(RiverEditingMode::Removing), m_isDragging(false),
		m_currentWaterLevel(0), m_waterEditing(false) {}
	HexMapBrush(std::initializer_list<BrushColor> list, unsigned int maxElevation, unsigned int maxBrushSize) : HexMapBrush()
	{
		m_colorPalette = list;
		m_maxElevation = maxElevation; 
		m_maxBrushSize = maxBrushSize;
	}
	std::vector<BrushColor>::size_type GetColorPalleteSize() const { return m_colorPalette.size(); }
	const std::string& GetColorName(std::vector<BrushColor>::size_type index) const { return m_colorPalette[index].colorName; }
	const glm::vec4& GetColor(std::vector<BrushColor>::size_type index) const { return m_colorPalette[index].color; }
	std::vector<BrushColor>::size_type GetCurrentColorIndex() const{ return m_currentColorIndex; }
	void SetCurrentColorIndex(std::vector<BrushColor>::size_type index)
	{
		if (index >= m_colorPalette.size())
			index = m_colorPalette.size() - 1;
		m_currentColorIndex = index; 
	}
	unsigned int GetCurrentElevation() const { return m_currentElevation; }
	unsigned int GetCurrentWaterLevel() const { return m_currentWaterLevel; }
	unsigned int GetCurrentBrushSize() const { return m_currentBrushSize; }
	unsigned int GetMaxElevation() const { return m_maxElevation; }
	unsigned int GetMaxBrushSize() const { return m_maxBrushSize; }
	std::vector<HexTile>::size_type GetCurrentTileIndex() const { return m_currentTileIndex; }
	std::vector<HexTile>::size_type GetPrevTileIndex() const { return m_prevTileIndex; }
	void SetCurrentTileIndex(std::vector<HexTile>::size_type index)
	{
		m_prevTileIndex = m_currentTileIndex;
		m_currentTileIndex = index;
	}
	void SetCurrentElevation(unsigned int elevation) 
	{ 
		if (elevation > m_maxElevation) 
			elevation = m_maxElevation; 
		m_currentElevation = elevation; 
	}
	void SetCurrentBrushSize(unsigned int size) 
	{ 
		if (size > m_maxBrushSize)
			size = m_maxBrushSize;
		m_currentBrushSize = size; 
	}
	void SetCurrentWaterLevel(unsigned int waterLevel)
	{
		m_currentWaterLevel = waterLevel;
	}
	void SetCurrentRiverEditingMode(RiverEditingMode mode)
	{
		m_riverEditingMode = mode;
	}
	void EnableColorEditing() { m_colorEditing = true; }
	void EnableElevationEditing() { m_elevationEditing = true; }
	void EnableRiverEditing() { m_riverEditing = true; }
	void EnableWaterEditing() { m_waterEditing = true; }
	void DisableWaterEditing() { m_waterEditing = false; }
	void DisableColorEditing() { m_colorEditing = false; }
	void DisableElevationEditing() { m_elevationEditing = false; }
	void DisableRiverEditing() { m_riverEditing = false; }
	bool CanColorEdit() const { return m_colorEditing; }
	bool CanElevationEdit() const { return m_elevationEditing; }
	bool CanRiverEdit() const { return m_riverEditing; }
	bool IsDragging() const { return m_isDragging; }
	bool CanWaterEdit() const { return m_waterEditing; }
	void SetDragging(bool b) { m_isDragging = b; }
	RiverEditingMode GetRiverEditingMode() const { return m_riverEditingMode; }
private:
	std::vector<BrushColor>::size_type m_currentColorIndex;
	std::vector<BrushColor> m_colorPalette;
	std::vector<HexTile>::size_type m_currentTileIndex;
	std::vector<HexTile>::size_type m_prevTileIndex;

	bool m_isDragging;
	bool m_colorEditing;
	bool m_elevationEditing;
	bool m_waterEditing;
	bool m_riverEditing;
	
	unsigned int m_currentElevation;
	unsigned int m_maxElevation;
	unsigned int m_currentWaterLevel;
	unsigned int m_currentBrushSize;
	unsigned int m_maxBrushSize;
	RiverEditingMode m_riverEditingMode;

};



#endif