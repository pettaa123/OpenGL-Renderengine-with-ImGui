#include "camera.h"

#include "core/Input.h"
#include "core/log.h"
#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#include <functional>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define M_PI 3.14159f

namespace Engine {

	Camera::Camera(const glm::mat4& projectionMatrix)
		: m_projectionMatrix(projectionMatrix)
	{
		// Sensible defaults
		m_panSpeed = 0.0015f;
		m_rotationSpeed = 0.002f;
		m_zoomSpeed = 0.2f;

		m_position = { -100, 100, 100 };
		m_rotation = glm::vec3(90.0f, 0.0f, 0.0f);

		m_focalPoint = glm::vec3(0.0f);
		m_distance = glm::distance(m_position, m_focalPoint);

		m_yaw = 3.0f * (float)M_PI / 4.0f;
		m_pitch = M_PI / 4.0f;
	}

	void Camera::focus()
	{
	}

	void Camera::onUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(GLFW_KEY_LEFT_ALT))
		{
			const glm::vec2& mouse{ Input::getMouseX(), Input::getMouseY() };
			glm::vec2 delta = mouse - m_initialMousePosition;
			m_initialMousePosition = mouse;

			if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
				mousePan(delta);
			else if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				mouseRotate(delta);
			else if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
				mouseZoom(delta.y);
		}

		m_position = calculatePosition();

		glm::quat orientation = getOrientation();
		m_rotation = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
		m_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1)) * glm::toMat4(glm::conjugate(orientation)) * glm::translate(glm::mat4(1.0f), -m_position);
	}
	void Camera::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(std::bind_front(&Camera::onMouseScrolled, this));
		dispatcher.dispatch<WindowResizeEvent>(std::bind_front(&Camera::onWindowResized, this));
	}

	void Camera::onResize(float width, float height)
	{
		setProjectionMatrix(glm::mat4(glm::perspectiveFov(glm::radians(45.0f), width, height, 0.1f, 10000.0f)));
	}

	bool Camera::onMouseScrolled(MouseScrolledEvent& e)
	{
		Log::info("MouseScrolledEvent");
		return true;
	}

	bool Camera::onWindowResized(WindowResizeEvent& e)
	{
		onResize((float)e.getWidth(), (float)e.getHeight());
		return false;
	}


	void Camera::mousePan(const glm::vec2& delta)
	{
		m_focalPoint += -getRightDirection() * delta.x * m_panSpeed * m_distance;
		m_focalPoint += getUpDirection() * delta.y * m_panSpeed * m_distance;
	}

	void Camera::mouseRotate(const glm::vec2& delta)
	{
		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		m_yaw += yawSign * delta.x * m_rotationSpeed;
		m_pitch += delta.y * m_rotationSpeed;
	}

	void Camera::mouseZoom(float delta)
	{
		m_distance -= delta * m_zoomSpeed;
		if (m_distance < 1.0f)
		{
			m_focalPoint += getForwardDirection();
			m_distance = 1.0f;
		}
	}

	glm::vec3 Camera::getUpDirection()
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::getRightDirection()
	{
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::getForwardDirection()
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Camera::calculatePosition()
	{
		return m_focalPoint - getForwardDirection() * m_distance;
	}

	glm::quat Camera::getOrientation()
	{
		return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
	}
}