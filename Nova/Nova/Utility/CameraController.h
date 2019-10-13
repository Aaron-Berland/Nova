#pragma once
#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H
#include <Nova/Renderer/Camera.h>
#include <Nova/Core/Timestep.h>
#include <Nova/Events/Event.h>
#include <Nova/Events/ApplicationEvent.h>
#include <Nova/Events/MouseEvent.h>


namespace Nova
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event &e);


		OrthographicCamera& GetCamera() { return m_camera; }
		const OrthographicCamera& GetCamera() const { return m_camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_aspectRatio;
		float m_zoomLevel = 1.0f;
		OrthographicCamera m_camera;

		bool m_rotation;

		glm::vec3 m_cameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_cameraRotation = 0.0f;
		float m_cameraTranslationSpeed = 5.0f, m_cameraRotationSpeed = 180.0f;
	};




}

#endif