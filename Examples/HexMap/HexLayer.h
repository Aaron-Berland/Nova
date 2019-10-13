#pragma once 
#ifndef HEXLAYER_H
#define HEXLAYER_H
#include <Nova/Layer.h>
#include <Nova/Core.h>
#include <Nova/Renderer/Texture.h>
#include <Nova/Renderer/Shader.h>
#include <memory>
#include "HexMap.h"
#include "HexMapBrush.h"
#include "HexMapCameraController.h"
#include <Nova/Renderer/Camera.h>
#include <Nova/Events/ApplicationEvent.h>
#include <Nova/Events/MouseEvent.h>


class HexLayer : public Nova::Layer
{
public:
	HexLayer();
	void OnImGuiRender() override;
	void OnUpdate(Nova::Timestep ts) override;
	void OnEvent(Nova::Event &e) override;

private:
	void RenderScene();
	void UpdateScene(Nova::Timestep ts);
	Nova::Scope<HexMap> m_hexMap;
	Nova::Ref<Nova::PerspectiveCamera> m_camera;
	Nova::Scope<HexMapCameraController> m_camController;
	HexMapBrush m_brush;
	Nova::Ref<Nova::Shader> m_hexShader;
	Nova::Ref<Nova::Shader> m_hexRiverShader;
	Nova::Ref<Nova::Shader> m_hexWaterShader;
	Nova::Ref<Nova::Shader> m_hexShoreShader;
	Nova::Ref<Nova::Shader> m_hexEstuaryShader;
	Nova::Ref<Nova::Texture2D> m_noiseTexture;
	glm::vec4 m_clearColor;
	
	float m_currentTime;
	int m_currentColorIndex;
	bool m_canColorEdit;
	bool m_canElevationEdit;
	bool m_canRiverEdit;
	bool m_canWaterEdit;
	int m_currentRiverEditingMode;
	int m_currentWaterLevel;
	int m_currentElevation;
	int m_currentBrushSize;
	unsigned int m_windowWidth, m_windowHeight;



	

	


};

#endif