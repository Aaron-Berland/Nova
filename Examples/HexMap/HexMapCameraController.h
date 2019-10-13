#pragma once
#ifndef HEXMAPCAMERACONTROLLER_H
#define HEXMAPCAMERACONTROLLER_H
#include <Nova/Core.h>
#include <Nova/Renderer/Camera.h>
#include <Nova/Core/Timestep.h>
#include <Nova/Events/Event.h>
#include <Nova/Events/ApplicationEvent.h>
#include <Nova/Events/MouseEvent.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
class HexMapCameraController
{
public:
	HexMapCameraController(const Nova::Ref<Nova::PerspectiveCamera> &camera,
		float cameraMoveSpeed, float cameraRotationSpeed, float cameraZoomSpeed,
		float minDistance, float maxDistance,
		float minAngle, float maxAngle,
		float zoom, float rotation) :
		m_camera(camera),
		m_cameraMoveSpeed(cameraMoveSpeed),
		m_cameraRotationSpeed(cameraRotationSpeed),
		m_cameraZoomSpeed(cameraZoomSpeed),
		m_minDistance(minDistance),
		m_maxDistance(maxDistance),
		m_minAngle(minAngle),
		m_maxAngle(maxAngle),
		m_zoom(zoom),
		m_rotation(rotation)
	{
		float angle = (1 - m_zoom)*m_minAngle + m_zoom*m_maxAngle;
		float distance = (1 - m_zoom)*m_maxDistance + m_zoom*m_minDistance;
		float rotCos = glm::cos(m_rotation);
		float rotSin = glm::sin(m_rotation);
		float angleCos = glm::cos(angle);
		float angleSin = glm::sin(angle);
		planeUp = glm::rotate(glm::vec3(0.0f,1.0f,0.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		planeRight = glm::rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_camera->SetUpAndFrontVector(glm::vec3(angleCos*rotSin, angleCos*rotCos, angleSin), glm::vec3(angleSin*rotSin, rotCos*angleSin, -angleCos));
		glm::vec3  pos = m_camera->GetPosition();
		m_camera->SetPosition(glm::vec3(pos.x, pos.y, distance));
	}


	void OnUpdate(Nova::Timestep ts);
	void OnEvent(Nova::Event &e);
private:
	bool OnWindowResizeEvent(Nova::WindowResizeEvent &e);
	bool OnMouseScrolledEvent(Nova::MouseScrolledEvent &e);
	void ApplyVerticalMovement(Nova::Timestep ts);
	void ApplyHorizontalMovement(Nova::Timestep ts);
	void ApplyZoomingOffset(float offset);
	void ApplyRotation(Nova::Timestep ts);

	Nova::Ref<Nova::PerspectiveCamera> m_camera;
	glm::vec3 planeUp;
	glm::vec3 planeRight;
	float m_cameraMoveSpeed;
	float m_cameraRotationSpeed;
	float m_cameraZoomSpeed;
	float m_minDistance;
	float m_maxDistance;
	float m_minAngle;
	float m_maxAngle;
	float m_zoom;
	float m_rotation;
};

#endif