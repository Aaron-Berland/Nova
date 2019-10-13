#include "HexLayer.h"
#include <Nova/Renderer/RenderCommand.h>
#include <Nova/Renderer/Renderer.h>
#include <Nova/Input.h>
#include <Nova/Window.h>
#include <Nova/Application.h>
#include <Nova/KeyCodes.h>
#include <Nova/MouseButtonCodes.h> 
#include <initializer_list>
#include <imgui.h>
#include <functional>
#include <Nova/Events/Event.h>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <Nova/Core/Benchmarking.h>
#include <Nova/Platform/OpenGLShader.h>
#include <Nova/Utility/Geometry.h>

HexLayer::HexLayer() 
	: m_currentTime(0.0f), m_camera(nullptr),m_hexMap(nullptr),m_hexShader(nullptr), m_clearColor(0.2f,0.2f,0.2f,1.0f),
	m_canElevationEdit(false), m_canColorEdit(true), m_canRiverEdit(false), m_canWaterEdit(false),
	m_currentColorIndex(0),m_currentElevation(0),m_currentWaterLevel(0), m_currentBrushSize(0), m_currentRiverEditingMode(0)
{

	float tileRadius = 5.5f;
	unsigned int tileCount = 30;
	float mapMidPoint = tileRadius*glm::sqrt(3.0f)*0.5f * tileCount * CHUNK_SIZE;
	float cameraFOV = 60.0f;
	unsigned int maxElevation = 12;
	unsigned int maxBrushSize = CHUNK_SIZE;
	float elevationStep = 2.15f;
	float riverElevationOffset = -1.5f;
	float cameraDistance = mapMidPoint / glm::tan(glm::radians(cameraFOV) / 2.0f);

	float cameraMoveSpeed = 2*35.0f;
	float cameraRotationSpeed = 1.0f;
	float cameraZoomSpeed = 0.01f;
	float minDistance = maxElevation * elevationStep * 2.0f;
	float maxDistance = std::max(cameraDistance*1.0f, 2.0f*minDistance);
	float minAngle = 0.0f;
	float maxAngle = glm::radians(80.0f);
	float zoom = 0.0f;
	float rotation = 0.0f;
	const Nova::Window& window = Nova::Application::Get().GetWindow();
	m_camera = Nova::CreateRef<Nova::PerspectiveCamera>(	glm::vec3(mapMidPoint, (2.0f / 3.0f) * mapMidPoint, cameraDistance),
															glm::vec3(mapMidPoint, (2.0f / 3.0f) * mapMidPoint, 0.0f),
															glm::vec3(0.0f, 1.0f, 0.0f), 
															cameraFOV, 
															(float)window.GetWidth() / window.GetHeight());

	m_camController = Nova::CreateScope<HexMapCameraController>(	m_camera,
		cameraMoveSpeed,
		cameraRotationSpeed,
		cameraZoomSpeed,
		minDistance,
		maxDistance,
		minAngle,
		maxAngle,
		zoom,
		rotation);
	HexTileMetrics tileMetrics(tileRadius, 0.8f,elevationStep,riverElevationOffset);
	std::vector<glm::vec4>defaultColors =
	{
		(1 / 255.0f)*glm::vec4(0.9f*243.0f,0.9f*235.0f,0.9f*0.0f,255.0f),
		(1 / 255.0f)*glm::vec4(0.0f, 0.9f*219.0f, 0.9f*37.0f, 255.0f),
		(1 / 255.0f)*glm::vec4(0.9f*239.0f, 0.9f*239.0f, 0.9f*239.0f, 255.0f),
		(1 / 255.0f)*glm::vec4(0.9f*41.0f, 0.9f*98.0f, 0.9f*255.0f, 255.0f)
	};
	m_brush = { { {defaultColors[0], "Yellow"},
												{defaultColors[1], "Green" },
												{defaultColors[2], "White"},
												{defaultColors[3], "Blue"} }, maxElevation, maxBrushSize};


	NoiseInfo noise(0.2f,1.0f, 1.0f);
	//std::vector<glm::vec4>defaultColors = { glm::vec4(0.8f,0.8f,0.8f,1.0f) };
	HexMapMetrics mapMetrics(tileMetrics, noise, glm::vec3(0.0, 0.0, 0.0f),tileCount, tileCount,maxElevation, 2, defaultColors);
	m_hexMap = Nova::CreateScope<HexMap>(mapMetrics);
	m_hexShader = Nova::Shader::Create("HexMap-Assets/Shaders/hexVS.vs", "HexMap-Assets/Shaders/hexFS.fs");
	m_hexRiverShader = Nova::Shader::Create("HexMap-Assets/Shaders/hexRiverVS.vs","HexMap-Assets/Shaders/hexRiverFS.fs");
	m_hexWaterShader = Nova::Shader::Create("HexMap-Assets/Shaders/hexWaterVS.vs","HexMap-Assets/Shaders/hexWaterFS.fs");
	m_hexShoreShader = Nova::Shader::Create("HexMap-Assets/Shaders/hexShoreVS.vs","HexMap-Assets/Shaders/hexShoreFS.fs");
	m_hexEstuaryShader = Nova::Shader::Create("HexMap-Assets/Shaders/hexEstuaryVS.vs", "HexMap-Assets/Shaders/hexEstuaryFS.fs");
	m_noiseTexture = Nova::Texture2D::Create("HexMap-Assets/Textures/noise.png");
	m_noiseTexture->Bind(0);
	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexRiverShader)->Use();
	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexRiverShader)->SetInt("u_noiseTexture", 0);

	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexWaterShader)->Use();
	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexWaterShader)->SetInt("u_noiseTexture", 0);


	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexShoreShader)->Use();
	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexShoreShader)->SetInt("u_noiseTexture", 0);

	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexEstuaryShader)->Use();
	std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexEstuaryShader)->SetInt("u_noiseTexture", 0);
}

void HexLayer::OnImGuiRender()
{
	ImGui::Begin("HexMap");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_clearColor));
	ImGui::End();

	ImGui::Begin("HexMap Brush Settings");
	ImGui::Text("General Settings");

	if (ImGui::SliderInt("Brush Size", &m_currentBrushSize, 0, m_brush.GetMaxBrushSize()))
	{
		m_brush.SetCurrentBrushSize((unsigned int)m_currentBrushSize);
	}

	ImGui::Separator();
	ImGui::Text("Color Editing Settings");
	if (ImGui::Checkbox("Color Editing Enable", &m_canColorEdit))
	{
		if (m_canColorEdit)
			m_brush.EnableColorEditing();
		else
			m_brush.DisableColorEditing();
	}
	for (decltype(m_brush.GetColorPalleteSize()) i = 0; i < m_brush.GetColorPalleteSize(); i++)
	{

		ImGui::RadioButton(m_brush.GetColorName(i).c_str(),&m_currentColorIndex, (int)i);
		ImGui::SameLine(100);
		const glm::vec4 &color = m_brush.GetColor(i);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.r, color.g, color.b, color.a));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(color.r, color.g, color.b, color.a));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.r, color.g, color.b, color.a));
		ImGui::Button("", ImVec2(20.0f,20.0f));
		ImGui::PopStyleColor(3);

	}
	ImGui::Separator();
	ImGui::Text("Elevation Editing Settings");
	if (ImGui::Checkbox("Elevation Editing Enable", &m_canElevationEdit))
	{
		if (m_canElevationEdit)
			m_brush.EnableElevationEditing();
		else
			m_brush.DisableElevationEditing();
	}
	if (ImGui::SliderInt("Elevation Value", &m_currentElevation, 0, m_brush.GetMaxElevation()))
	{
		m_brush.SetCurrentElevation((unsigned int)m_currentElevation);
	}
	
	ImGui::Separator();
	ImGui::Text("River Editing Settings");
	if (ImGui::Checkbox("River Editing Enable", &m_canRiverEdit))
	{
		if (m_canRiverEdit)
			m_brush.EnableRiverEditing();
		else
			m_brush.DisableRiverEditing();
	}
	ImGui::Text("River Editing Mode:");
	ImGui::RadioButton("Removing Rivers", &m_currentRiverEditingMode, 0);
	ImGui::RadioButton("Adding Rivers", &m_currentRiverEditingMode, 1);
	ImGui::Separator();
	ImGui::Text("Water Level Settings");
	if (ImGui::Checkbox("Water Level Editing Enable", &m_canWaterEdit))
	{
		if (m_canWaterEdit)
			m_brush.EnableWaterEditing();
		else
			m_brush.DisableWaterEditing();
	}
	if (ImGui::SliderInt("Water Level Value", &m_currentWaterLevel, 0, m_brush.GetMaxElevation()))
	{
		m_brush.SetCurrentWaterLevel((unsigned int)m_currentWaterLevel);
	}

	ImGui::Separator();
	ImGui::Text("Current Tile Info");
	ImGui::Text((std::string("Tile Index : ") + std::to_string(m_brush.GetCurrentTileIndex())).c_str());
	const HexTile& tile = m_hexMap->GetTile(m_brush.GetCurrentTileIndex());
	ImGui::Text((std::string("Elevation : ") + std::to_string(tile.elevation)).c_str());
	ImGui::Text((std::string("Water Level : ") + std::to_string(tile.waterLevel)).c_str());
	ImGui::Text("Color :");
	ImGui::SameLine(50);
	const glm::vec4 &color = tile.color;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.r, color.g, color.b, color.a));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(color.r, color.g, color.b, color.a));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.r, color.g, color.b, color.a));
	ImGui::Button("", ImVec2(20.0f, 20.0f));
	ImGui::PopStyleColor(3);
	if (tile.HasIncomingRiver())
		ImGui::Text((std::string("Tile has Incoming river with direction : ") + std::to_string(tile.inComingRiver)).c_str());
	if (tile.HasOutGoingRiver())
		ImGui::Text((std::string("Tile has Outgoing river with direction : ") + std::to_string(tile.outGoingRiver)).c_str());

	
	
	

	ImGui::End();
}

void HexLayer::OnUpdate(Nova::Timestep ts)
{

	UpdateScene(ts);
	RenderScene();

	
}

void HexLayer::OnEvent(Nova::Event &e)
{
	m_camController->OnEvent(e);
}

void HexLayer::UpdateScene(Nova::Timestep ts)
{
	



	m_currentTime += ts;
	m_camController->OnUpdate(ts);

	if (Nova::Input::IsMouseButtonPressed(NOVA_MOUSE_BUTTON_1) && !(ImGui::GetIO().WantCaptureMouse))
	{

		
			float x, y;
			Nova::Input::GetMousePosition(x, y);
			if (y < 0.0f)
				Nova::Input::GetMousePosition(x, y);
			const Nova::Window& window = Nova::Application::Get().GetWindow();
			unsigned int width = window.GetWidth();
			unsigned int height = window.GetHeight();
			Nova::Ray cameraRay = m_camera->GetCameraRay(width, height, x, y);
			HexTileRayHitInfo tileHitInfo = m_hexMap->GetHexTileInfo(cameraRay);
			if (tileHitInfo.HasHitted())
			{
				m_brush.SetCurrentTileIndex(tileHitInfo.hexIndex);
				m_hexMap->EditMap(m_brush);
				m_brush.SetDragging(true);
				//NOVA_TRACE("HexTile Index : {0}", ray.hexIndex);
			}
			else
			{
				m_brush.SetDragging(false);
			}
		
	

	}
	else
	{
		m_brush.SetDragging(false);
	}
	m_brush.SetCurrentColorIndex((decltype(m_brush.GetColorPalleteSize())) m_currentColorIndex);
	m_brush.SetCurrentRiverEditingMode(IntToRiverEditingMode(m_currentRiverEditingMode));
}


void HexLayer::RenderScene()
{
	Nova::RenderCommand::SetClearColor(m_clearColor);
	Nova::RenderCommand::Clear();

	Nova::Renderer::BeginScene(*m_camera);


		Nova::Renderer::SetShaderData(*m_hexShader);
		unsigned int i = 0;
		for (auto it = m_hexMap->cTerrainVaoBegin(); it != m_hexMap->cTerrainVaoEnd(); it++, i++)
			Nova::Renderer::UnIndexedBasicSubmit(*(*it));




		Nova::Renderer::SetShaderData(*m_hexShoreShader);
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexShoreShader)->SetFloat("time", (float)m_currentTime);
		i = 0;
		for (auto it = m_hexMap->cShoreVaoBegin(); it != m_hexMap->cShoreVaoEnd(); it++, i++)
			Nova::Renderer::UnIndexedBasicSubmit(*(*it));



		Nova::Renderer::SetShaderData(*m_hexWaterShader);
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexWaterShader)->SetFloat("time", (float)m_currentTime);
		i = 0;
		for (auto it = m_hexMap->cWaterVaoBegin(); it != m_hexMap->cWaterVaoEnd(); it++, i++)
			Nova::Renderer::UnIndexedBasicSubmit(*(*it));

		Nova::Renderer::SetShaderData(*m_hexRiverShader);
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexRiverShader)->SetFloat("time", (float)m_currentTime);
		i = 0;
		for (auto it = m_hexMap->cRiverVaoBegin(); it != m_hexMap->cRiverVaoEnd(); it++, i++)
			Nova::Renderer::UnIndexedBasicSubmit(*(*it));


		Nova::Renderer::SetShaderData(*m_hexEstuaryShader);
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_hexEstuaryShader)->SetFloat("time", (float)m_currentTime);
		i = 0;
		for (auto it = m_hexMap->cEstuaryVaoBegin(); it != m_hexMap->cEstuaryVaoEnd(); it++, i++)
			Nova::Renderer::UnIndexedBasicSubmit(*(*it));


	Nova::Renderer::EndScene();
	
}


