#pragma once

#include "engine/renderer/orthographicCamera.h"
#include "engine/core/timestep.h"

#include "engine/event/ApplicationEvent.h"
#include "engine/event/MouseEvent.h"

namespace Engine {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		void onResize(float width, float height);

		OrthographicCamera& getCamera() { return m_camera; }
		const OrthographicCamera& getCamera() const { return m_camera; }

		float getZoomLevel() const { return m_zoomLevel; }
		void setZoomLevel(float level) { m_zoomLevel = level; }
	private:
		bool onMouseScrolled(MouseScrolledEvent& e);
		bool onWindowResized(WindowResizeEvent& e);
	private:
		float m_aspectRatio;
		float m_zoomLevel = 1.0f;
		OrthographicCamera m_camera;

		bool m_rotation;

		glm::vec3 m_cameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_cameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_cameraTranslationSpeed = 5.0f, m_cameraRotationSpeed = 180.0f;
	};

}