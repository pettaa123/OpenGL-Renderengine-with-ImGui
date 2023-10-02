#include "engine/renderer/camera.h"
#include "engine/core/Input.h"
#include "engine/core/log.h"
#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#include <functional>


#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp> 


namespace Engine {

	Camera::Camera(const glm::mat4& projectionMatrix)
		: m_projectionMatrix(projectionMatrix)
	{
		// Sensible defaults
		m_panSpeed = 0.005f;
		m_rotationSpeed = 0.004f;
		m_zoomSpeed = 0.3f;

		m_position = { 0, 0, -100 };
		m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		m_focalPoint = glm::vec3(0.0f);
		m_distance = glm::distance(m_position, m_focalPoint);

		m_yaw = 0.0f;
		m_pitch = 0.0f;
		m_roll = 0.0f; // Initial roll angle

		m_z_min = 0.01f; // min and max z-distance considered in clip space
		m_z_max = 5.0f;
	}

	void Camera::focus()
	{
	}

	std::optional<glm::vec3> Camera::unproject(glm::vec2 mouse, float winZ) {

		std::cout << glm::to_string(m_viewMatrix);
		std::cout << glm::to_string(m_projectionMatrix);

		glm::mat4 A =  m_projectionMatrix * m_viewMatrix;

		A=glm::inverse(A);

		int h = 1080;
		int w = 1920;

		// Transformation of normalized coordinates between -1 and 1

		//glm::vec4 = {
		//	 mouse.X / viewport.Width * 2.0f - 1.0f,
		//	  (viewport.Height - mouse.Y) / viewport.Height * 2.0f - 1.0f,
		//	  2.0f * winZ - 1.0f,
		//	  1.0f
		//};

		//auto& window = Application::s_get().getWindow();
		//auto h = window.getHeight();
		//auto w = window.getHeight();

		glm::vec4 vIn = {
			mouse.x / w * 2.0f - 1.0f,
			(h - mouse.y) / h * 2.0f - 1.0f,
			2.0f * winZ - 1.0f,
			1.0f
		};


		// Objects coordinates
		glm::vec4 vOut = A * vIn;

		if (vOut.w == 0) {
			return {};
		}

		vOut /= vOut.w;
		return glm::vec3{ vOut.x,vOut.y,vOut.z };
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
		m_rotation = glm::eulerAngles(orientation) * (glm::radians(180.0f));
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
		// Ensure pitch stays within bounds to prevent camera flipping
		if (m_pitch > glm::radians(90.0f) - glm::radians(0.5f))
			m_pitch = glm::radians(90.0f) - glm::radians(0.5f);
		if (m_pitch < -glm::radians(90.0f) - glm::radians(0.5f))
			m_pitch = -glm::radians(90.0f) - glm::radians(0.5f);
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

	void Camera::calcProjectionMatrixFromIntrinsics() {
		if (!m_intrinsics.has_value()) {
			Log::error("Load Intrinsics first");
			return;
		}
		m_projectionMatrix = glm::mat4(
			2.0f * m_intrinsics->fx / m_intrinsics->width,
			0.0f ,
			2.0f * (m_intrinsics->cx + 0.5f) / m_intrinsics->width - 1.0f,
			0.0f,
			0.0f,
			2.0f * m_intrinsics->fy / m_intrinsics->height,
			2.0f * (m_intrinsics->cy + 0.5f) / float(m_intrinsics->height) - 1.0f,
			0.0f,
			0.0f,
			0.0f,
			(m_z_max + m_z_min) / (m_z_max - m_z_min),
			-2.0f * m_z_max * m_z_min / (m_z_max - m_z_min),
			0.0f,
			0.0f,
			1.0f,
			0.0f);
	}
}