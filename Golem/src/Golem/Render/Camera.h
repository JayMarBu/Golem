#pragma once

namespace golem
{
	class Camera
	{
		// Members ********************************************************************************
	public:

	protected:
		glm::mat4 m_projectionMatrix{ 1.f };
		glm::mat4 m_viewMatrix{ 1.0f };

		// Methods ********************************************************************************
	public:

		void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.f, -1.0f, 0.f));
		void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.f, -1.0f, 0.f));
		void SetViewXYZ(glm::vec3 position, glm::vec3 rotation);

		void SetOthographicProjection(float left, float right, float top, float bottom, float near, float far);

		inline void SetOthographicProjection(glm::vec2 leftRight, glm::vec2 topBottom, glm::vec2 nearFar)
		{
			SetOthographicProjection(leftRight.x, leftRight.y, topBottom.x, topBottom.y, nearFar.x, nearFar.y);
		}

		inline void SetOthographicProjection(glm::vec3 leftBottomNear, glm::vec3 rightTopFar)
		{
			SetOthographicProjection(leftBottomNear.x, rightTopFar.x, rightTopFar.y, leftBottomNear.y, leftBottomNear.z, rightTopFar.z);
		}

		void SetPerspectiveProjection(float FOVy, float aspect, float near, float far);

		inline void SetPerspectiveProjection(float FOVy, float aspect, glm::vec2 nearFar)
		{
			SetPerspectiveProjection(FOVy, aspect, nearFar.x, nearFar.y);
		}

		const glm::mat4& GetProjection() const { return m_projectionMatrix; }
		const glm::mat4& GetView() const { return m_viewMatrix; }
	};
}