#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <Nova/Utility/Geometry.h>
namespace Nova
{
	class Camera
	{
	public:
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;
		virtual Ray GetCameraRay(unsigned int w, unsigned int h, float xMouse, float yMouse) const = 0;
	};


	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		const glm::vec3& GetPosition() const { return m_position; }

		void SetPosition(const glm::vec3& position) { m_position = position; RecalculateViewMatrix(); }
		void SetRotation(float rotation) { m_rotation = rotation; RecalculateViewMatrix(); }
		void SetProjection(float left, float right, float bottom, float top);
		const glm::mat4& GetProjectionMatrix() const override{ return m_projectionMatrix; }
		const glm::mat4& GetViewMatrix() const override { return m_viewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const override { return m_viewProjectionMatrix; }
		Ray GetCameraRay(unsigned int w, unsigned int h, float xMouse, float yMouse) const { return Ray(); }
		

	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

		glm::vec3 m_position;
		float m_rotation;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up, 
							float fov = 45.0f, float aspectRatio = 1.0f, float nearPlane = 1.0f, float farPlane = 1300.0f);
		void ApplyFrontMovement(float amount);
		void ApplyHorizontalMovement(float amount);
		void ApplyVerticalMovement(float amount);
		void ApplyPositionOffset(const glm::vec3 &offset);
		void ApplyRotationAroundAxis(float angle, const glm::vec3 &axis);
		void ApplyCameraRotation(const glm::vec2 &offsets);
		void ApplyCameraRoll(float amount);
		void SetUpAndFrontVector(const glm::vec3 &up, const glm::vec3 &front);
		void SetPosition(const glm::vec3 &pos);
		const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }
		const glm::vec3& GetPosition() const { return m_position; }
		const glm::vec3& GetUpVector() const { return m_up; }
		const glm::vec3& GetFrontVector() const { return m_front; }
		const glm::vec3& GetRightVector() const { return m_right; }
		float GetNearClippingPlane() const { return m_nearPlane; }
		float GetFOV() const { return m_fov; }
		float GetFOVY() const { return m_fovy; }
		void SetAspectRatio(float aspect);
		Ray GetCameraRay(unsigned int width, unsigned int height, float xMouse, float yMouse) const 
		{

			yMouse = height - yMouse;
			xMouse -= (float)width / 2.0f;
			yMouse -= (float)height / 2.0f;
			xMouse /= ((float)width / 2.0f);
			yMouse /= ((float)height / 2.0f);
			float r = xMouse * glm::tan(m_fov / 2.0f) * m_nearPlane;
			float t = yMouse * glm::tan(m_fovy / 2.0f) * m_nearPlane;
			glm::vec3 origin = m_position + m_nearPlane*m_front+ r * m_right + t* m_up;
			glm::vec3 direction = glm::normalize(origin - m_position);
			return Ray(origin, direction);
		}
	private:
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		float m_fov;
		float m_fovy;
		float m_aspectRatio;
		float m_nearPlane;
		float m_farPlane;

		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

	};

}

#endif