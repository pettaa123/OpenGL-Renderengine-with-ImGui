#include "renderer/OrthographicCameraController.h"

#include "core/input.h"
#include "core/keyCodes.h"
#include <functional>

namespace Engine {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation)
	{
	}

	void OrthographicCameraController::onUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(Key::A))
		{
			m_cameraPosition.x -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		else if (Input::isKeyPressed(Key::D))
		{
			m_cameraPosition.x += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		if (Input::isKeyPressed(Key::W))
		{
			m_cameraPosition.x += -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		else if (Input::isKeyPressed(Key::S))
		{
			m_cameraPosition.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		if (m_rotation)
		{
			if (Input::isKeyPressed(Key::Q))
				m_cameraRotation += m_cameraRotationSpeed * ts;
			if (Input::isKeyPressed(Key::E))
				m_cameraRotation -= m_cameraRotationSpeed * ts;

			if (m_cameraRotation > 180.0f)
				m_cameraRotation -= 360.0f;
			else if (m_cameraRotation <= -180.0f)
				m_cameraRotation += 360.0f;

			m_camera.setRotation(m_cameraRotation);
		}

		m_camera.setPosition(m_cameraPosition);

		m_cameraTranslationSpeed = m_zoomLevel;
	}

	void OrthographicCameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(std::bind_front(&OrthographicCameraController::onMouseScrolled,this));
		dispatcher.dispatch<WindowResizeEvent>(std::bind_front(&OrthographicCameraController::onWindowResized,this));
	}

	void OrthographicCameraController::onResize(float width, float height)
	{
		m_aspectRatio = width / height;
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e)
	{
		m_zoomLevel -= e.getYOffset() * 0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizeEvent& e)
	{
		onResize((float)e.getWidth(), (float)e.getHeight());
		return false;
	}

}