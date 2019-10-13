#include "Camera.h"
#include <Nova/Core.h>
#include <Nova/Log.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
namespace Nova
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_position(0.0f,0.0f,0.0f), m_rotation(0.0f),m_viewMatrix(1.0f),m_projectionMatrix(glm::ortho(left,right,bottom,top,-1.0f,1.0f))
	{
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		m_viewMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-m_rotation),glm::vec3(0.0f,0.0f,1.0f))*glm::translate(glm::mat4(1.0f), -m_position);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;


	}

	void PerspectiveCamera::UpdateViewMatrix()
	{
		m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}
	
	void PerspectiveCamera::UpdateProjectionMatrix() 
	{
		m_projectionMatrix = glm::perspective(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}


	PerspectiveCamera::PerspectiveCamera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up, float fov /*= 45.0f*/, float aspectRatio /*= 1.0f*/, float nearPlane /*= 0.1f*/, float farPlane /*= 10000.0f*/)
		: m_position(pos), m_fov(glm::radians(fov)), m_aspectRatio(aspectRatio), m_nearPlane(nearPlane), m_farPlane(farPlane)
	{
		NOVA_CORE_ASSERT(aspectRatio > 0, "Camera aspect ratio is negative!!");
		NOVA_CORE_ASSERT(m_fov > 0.0f && m_fov < glm::radians(180.0f), "Camera Field of View out of range");
		NOVA_CORE_ASSERT(glm::distance(pos, target) > 0, "Camera position and target are the same!!!");
		m_front = glm::normalize(target - pos);
		NOVA_CORE_ASSERT(glm::length2(glm::cross(m_front, up))>0, "Camera front and up vector are colinear!!");
		if (glm::dot(up, m_front) != 0.0f)
		{
			m_up = glm::normalize(up - glm::dot(up, m_front)*m_front);
		}
		else
		{
			m_up = up;
		}
		m_right = glm::normalize(glm::cross(m_front, m_up));
		m_fovy = 2.0f*glm::atan(glm::tan(m_fov/ 2.0f) / m_aspectRatio);

		

		UpdateViewMatrix();
		UpdateProjectionMatrix();

	}

	void PerspectiveCamera::ApplyHorizontalMovement(float amount)
	{
		m_position += amount * m_right;
		UpdateViewMatrix();
	}

	void PerspectiveCamera::ApplyVerticalMovement(float amount)
	{
		m_position += amount * m_up;
		UpdateViewMatrix();
	}

	void PerspectiveCamera::ApplyPositionOffset(const glm::vec3 &offset)
	{
		m_position += offset;
		UpdateViewMatrix();
	}

	void PerspectiveCamera::ApplyRotationAroundAxis(float angle, const glm::vec3 &axis)
	{
		m_up = glm::rotate(m_up, angle, axis);
		m_front = glm::rotate(m_front, angle, axis);
		m_right = glm::rotate(m_right, angle, axis);
		UpdateViewMatrix();
	}

	void PerspectiveCamera::ApplyFrontMovement(float amount)
	{
		m_position += amount * m_front;
		UpdateViewMatrix();
	}

	void PerspectiveCamera::ApplyCameraRoll(float amount)
	{
		m_up = glm::rotate(m_up, amount, m_front);
		m_right = glm::normalize(glm::cross(m_front, m_up));
		UpdateViewMatrix();
	}

	void PerspectiveCamera::SetUpAndFrontVector(const glm::vec3 &up, const glm::vec3 &front)
	{
		m_front = glm::normalize(front);
		NOVA_CORE_ASSERT(glm::length2(glm::cross(m_front, up)) > 0, "Camera front and up vector are colinear!!");
		m_up = glm::normalize(up - glm::dot(up, m_front)*m_front);
		m_right = glm::normalize(glm::cross(m_front, m_up));
		UpdateViewMatrix();
	}

	void PerspectiveCamera::SetPosition(const glm::vec3 &pos)
	{
		m_position = pos;
		UpdateViewMatrix();
	}

	void PerspectiveCamera::SetAspectRatio(float aspect)
	{
		m_aspectRatio = aspect;
		m_fovy = 2.0f*glm::atan(glm::tan(m_fov / 2.0f) / m_aspectRatio);
		UpdateProjectionMatrix();
	}

	void PerspectiveCamera::ApplyCameraRotation(const glm::vec2 &offsets)
	{
		if (offsets.x == 0.0f && offsets.y == 0.0f)
			return;
		if (offsets.x != 0.0f)
		{
			m_front = glm::rotate(m_front, offsets.x, m_up);
			m_right = glm::normalize(glm::cross(m_front, m_up));
		}
		if (offsets.y != 0.0f)
		{
			m_front = glm::rotate(m_front, offsets.y, m_right);
			m_up = glm::rotate(m_up, offsets.y,m_right);
		}
		UpdateViewMatrix();
	}

}