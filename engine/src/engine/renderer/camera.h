#pragma once

#include <glm/glm.hpp>
#include <optional>
#include "engine/core/timestep.h"
#include "engine/event/ApplicationEvent.h"
#include "engine/event/MouseEvent.h"



namespace Engine {

	struct Intrinsics {
		float fx, fy;
		float cx, cy;
		int width, height;
	};

	class Camera
	{
	public:
		Camera(const glm::mat4& projectionMatrix);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		void onResize(float width, float height);

		void focus();

		inline float getDistance() const { return m_distance; }
		inline void setDistance(float distance) { m_distance = distance; }

		inline void setProjectionMatrix(const glm::mat4& projectionMatrix) { m_projectionMatrix = projectionMatrix; }


		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		void calcProjectionMatrixFromIntrinsics();

		glm::vec3 getUpDirection();
		glm::vec3 getRightDirection();
		glm::vec3 getForwardDirection();
		const glm::vec3& getPosition() const { return m_position; }


		/// <summary>
		/// Unprojects the mouse coordinates.
		/// https://dondi.lmu.build/share/cg/unproject-explained.pdf
		/// </summary>
		/// <param name="view">The view.</param>
		/// <param name="projection">The projection.</param>
		/// <param name="viewport">The viewport.</param>
		/// <param name="mouse">The mouse.</param>
		/// <param name="winZ">The win z.</param>
		/// <returns></returns>
		std::optional<glm::vec3> unproject(glm::vec2 mouse, float winZ);


	private:
		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition();
		glm::quat getOrientation();
	private:

		bool onMouseScrolled(MouseScrolledEvent& e);
		bool onWindowResized(WindowResizeEvent& e);

		glm::mat4 m_projectionMatrix; //projects 3d data into clip space
		glm::mat4 m_viewMatrix;
		glm::vec3 m_position, m_rotation, m_focalPoint;

		bool m_panning, m_rotating;
		glm::vec2 m_initialMousePosition;

		float m_distance;
		float m_panSpeed, m_rotationSpeed, m_zoomSpeed;

		float m_pitch, m_yaw, m_roll;

		float m_z_min, m_z_max;

		std::optional<Intrinsics> m_intrinsics;
	};

}
