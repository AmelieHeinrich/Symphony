#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <events/Event.h>
#include <core/Input.h>
#include <core/Log.h>

using namespace symphony;

class EditorCamera
{
public:
	EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), m_Projection(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	void OnUpdate(float dt)
	{
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		m_InitialMousePosition = mouse;

		UpdateView();
	}

	void ProcessInput(float dt)
	{
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;

		if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
			MouseRotate(delta);
		else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
			MousePan(delta);

		UpdateView();
	}

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(SY_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	inline float GetDistance() const { return m_Distance; }
	inline void SetDistance(float distance) { m_Distance = distance; }

	inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

	glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
	glm::mat4 GetProjectionMatrix() const { return m_Projection; }
	glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

	glm::vec3 GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	const glm::vec3& GetPosition() const { return m_Position; }

	glm::quat GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }
private:
	void UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void UpdateView()
	{
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	bool OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	std::pair<float, float> PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float RotationSpeed() const
	{
		return 0.8f;
	}

	float ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}
private:
	float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

	glm::mat4 m_Projection;
	glm::mat4 m_ViewMatrix;
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

	glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

	float m_Distance = 10.0f;
	float m_Pitch = 0.0f, m_Yaw = 0.0f;

	float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};