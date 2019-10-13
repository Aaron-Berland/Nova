#include "HexMapCameraController.h"
#include <Nova/Input.h>
#include <Nova/KeyCodes.h>
#include <imgui.h> 

void HexMapCameraController::OnUpdate(Nova::Timestep ts)
{
	if (Nova::Input::IsKeyPressed(NOVA_KEY_A))
		ApplyHorizontalMovement(-ts);
	else if (Nova::Input::IsKeyPressed(NOVA_KEY_D))
		ApplyHorizontalMovement(ts);
	if (Nova::Input::IsKeyPressed(NOVA_KEY_W))
		ApplyVerticalMovement(ts);
	else if (Nova::Input::IsKeyPressed(NOVA_KEY_S))
		ApplyVerticalMovement(-ts);

	if (Nova::Input::IsKeyPressed(NOVA_KEY_Q))
		ApplyRotation(ts);
	else if (Nova::Input::IsKeyPressed(NOVA_KEY_E))
		ApplyRotation(-ts);
}

void HexMapCameraController::OnEvent(Nova::Event &e)
{
	Nova::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Nova::WindowResizeEvent>(std::bind(&HexMapCameraController::OnWindowResizeEvent, this, std::placeholders::_1));
	dispatcher.Dispatch<Nova::MouseScrolledEvent>(std::bind(&HexMapCameraController::OnMouseScrolledEvent, this, std::placeholders::_1));
}

void HexMapCameraController::ApplyVerticalMovement(Nova::Timestep ts)
{
	float speedAdjustment = 1.0f + (1 - m_zoom)*3.0f;
	m_camera->ApplyPositionOffset(ts*m_cameraMoveSpeed*planeUp*speedAdjustment);
}

void HexMapCameraController::ApplyHorizontalMovement(Nova::Timestep ts)
{
	float speedAdjustment = 1.0f + (1 - m_zoom)*3.0f;
	m_camera->ApplyPositionOffset(ts*m_cameraMoveSpeed*planeRight*speedAdjustment);
}

void HexMapCameraController::ApplyZoomingOffset(float offset)
{
	m_zoom = glm::clamp(m_zoom + offset*m_cameraZoomSpeed, 0.0f, 1.0f);
	float angle = (1 - m_zoom)*m_minAngle + m_zoom*m_maxAngle;
	float t = m_zoom;
	t = t * (2 - t);
	float distance = t*m_minDistance +(1-t)*m_maxDistance;
	float rotCos = glm::cos(m_rotation);
	float rotSin = glm::sin(m_rotation);
	float angleCos = glm::cos(angle);
	float angleSin = glm::sin(angle);
	m_camera->SetUpAndFrontVector(glm::vec3(-angleCos*rotSin, angleCos*rotCos, angleSin),glm::vec3(-angleSin*rotSin, rotCos*angleSin, -angleCos));
	glm::vec3  pos = m_camera->GetPosition();
	m_camera->SetPosition(glm::vec3(pos.x, pos.y, distance));
}

void HexMapCameraController::ApplyRotation(Nova::Timestep ts)
{
	m_rotation += m_cameraRotationSpeed*ts;
	m_camera->ApplyRotationAroundAxis(m_cameraRotationSpeed*ts, glm::vec3(0.0f, 0.0f, 1.0f));
	planeUp = glm::rotate(glm::vec3(0.0f, 1.0f, 0.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	planeRight = glm::rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
}


bool HexMapCameraController::OnWindowResizeEvent(Nova::WindowResizeEvent &e)
{
	m_camera->SetAspectRatio((float)e.GetWidth() / e.GetHeight());
	return false;
}

bool HexMapCameraController::OnMouseScrolledEvent(Nova::MouseScrolledEvent &e)
{
	if (!ImGui::GetIO().WantCaptureMouse)
		ApplyZoomingOffset(e.GetYOffset());
	return false;
}