#include "CameraController.h"

#include <Nova/KeyCodes.h>
#include <Nova/Input.h>


namespace Nova
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation) :
	m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio*m_zoomLevel, m_aspectRatio * m_zoomLevel,-m_zoomLevel,m_zoomLevel), m_rotation(rotation)
	{

	}
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(NOVA_KEY_A))
		{
			m_cameraPosition.x -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(NOVA_KEY_D))
		{
			m_cameraPosition.x += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(NOVA_KEY_W))
		{
			m_cameraPosition.x += -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(NOVA_KEY_S))
		{
			m_cameraPosition.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		if (m_rotation)
		{
			if (Input::IsKeyPressed(NOVA_KEY_Q))
				m_cameraRotation += m_cameraRotationSpeed * ts;
			if (Input::IsKeyPressed(NOVA_KEY_E))
				m_cameraRotation -= m_cameraRotationSpeed * ts;

			m_camera.SetRotation(m_cameraRotation);
		}

		m_camera.SetPosition(m_cameraPosition);

		m_cameraTranslationSpeed = m_zoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event &e) 
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&OrthographicCameraController::OnWindowResized, this, std::placeholders::_1));
	    dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&OrthographicCameraController::OnMouseScrolled, this, std::placeholders::_1));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_zoomLevel -= e.GetYOffset() * 0.25f;
		if (m_zoomLevel < 0.25f)
			m_zoomLevel = 0.25f;
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}


	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

}