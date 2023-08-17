#pragma once

#include <glm/glm.hpp>

namespace Engine {

	class Camera
	{
	public:
		Camera(const glm::mat4& projectionMatrix);

		void focus();
		void update();

		inline float getDistance() const { return m_distance; }
		inline void setDistance(float distance) { m_distance = distance; }

		inline void setProjectionMatrix(const glm::mat4& projectionMatrix) { m_projectionMatrix = projectionMatrix; }

		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }

		glm::vec3 getUpDirection();
		glm::vec3 getRightDirection();
		glm::vec3 getForwardDirection();
		const glm::vec3& getPosition() const { return m_position; }
	private:
		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition();
		glm::quat getOrientation();
	private:
		glm::mat4 m_projectionMatrix, m_viewMatrix;
		glm::vec3 m_position, m_rotation, m_focalPoint;

		bool m_panning, m_rotating;
		glm::vec2 m_initialMousePosition;
		glm::vec3 m_initialFocalPoint, m_InitialRotation;

		float m_distance;
		float m_panSpeed, m_rotationSpeed, m_zoomSpeed;

		float m_pitch, m_yaw;
	};

}
