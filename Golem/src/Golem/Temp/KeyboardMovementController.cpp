#include "golpch.h"
#include "KeyboardMovementController.h"

#include "../Input/Input.h"
#include "../Utils.h"
#include "glm/common.hpp"

#include "imgui.h"
#include "../Application.h"
#include "GLFW/glfw3.h"

namespace golem
{

	void KeyboardMovementController::MoveInPlaneXZ(float dt, GameObject gameObject, glm::vec2 viewCentre)
	{
		GOL_PROFILE_FUNCTION();

		auto& transform = gameObject.GetComponent<Transform>();

		glm::vec3 rotate{ 0.0f };

		if (Input::IsKeyPressed(keys.lookRight))
			rotate.y += 1;
		if (Input::IsKeyPressed(keys.lookLeft))
			rotate.y -= 1;
		if (Input::IsKeyPressed(keys.lookUp))
			rotate.x += 1;
		if (Input::IsKeyPressed(keys.lookDown))
			rotate.x -= 1;

		if (Input::IsMouseButtonPressed(MouseCode::BUTTON_RIGHT))
		{
			GOL_PROFILE_SCOPE("KeyboardMovementController::MoveInPlaneXZ -> mouse button down");
		
			auto window = Application::Get().GetWindowPtr();

			static int winXPos, winYPos;
			{
				GOL_PROFILE_SCOPE("KeyboardMovementController::MoveInPlaneXZ -> mouse button down -> get window pos");
				glfwGetWindowPos(static_cast<GLFWwindow*>(window->GetNativeWindow()), &winXPos, &winYPos);
			}

			{
				GOL_PROFILE_SCOPE("KeyboardMovementController::MoveInPlaneXZ -> mouse button down -> calculate deltas");
				deltax = (Input::GetMouseX() + winXPos) - (glm::floor(viewCentre.x));
				deltay = (Input::GetMouseY() + winYPos) - (glm::floor(viewCentre.y));

				rotate.y += turnSpeed * dt * (float)deltax;
				rotate.x -= turnSpeed * dt * (float)deltay;
			}

			//Input::SetMousePosCentre();
			Input::SetMousePos(viewCentre.x - winXPos, viewCentre.y - winYPos);
		}

		{
			GOL_PROFILE_SCOPE("KeyboardMovementController::MoveInPlaneXZ -> transform rotations");
		
			if (VecIsZero(rotate))
			{
				glm::vec3 rot = transform.EulerAngles() + turnSpeed * dt * glm::normalize(rotate);
				transform.Rotation(rot);
			}
			
		
			transform.RotationX(glm::clamp(transform.EulerAngles().x, -1.5f, 1.5f));
			transform.RotationY(glm::mod(transform.EulerAngles().y, glm::two_pi<float>()));

		}

		GOL_PROFILE_SCOPE("KeyboardMovementController::MoveInPlaneXZ -> translation");

		float yaw = transform.EulerAngles().y;
		const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0 };

		if (Input::IsKeyPressed(keys.moveRight))
			moveDir += rightDir;
		if (Input::IsKeyPressed(keys.moveLeft))
			moveDir -= rightDir;
		if (Input::IsKeyPressed(keys.moveForward))
			moveDir += forwardDir;
		if (Input::IsKeyPressed(keys.moveBack))
			moveDir -= forwardDir;
		if (Input::IsKeyPressed(keys.moveUp))
			moveDir += upDir;
		if (Input::IsKeyPressed(keys.moveDown))
			moveDir -= upDir;

		if (VecIsZero(moveDir))
			transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}

	void KeyboardMovementController::MoveInPlaneXY(float dt, GameObject gameObject)
	{
		const glm::vec3 forwardDir{ 0, 0.0f, 1 };
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0 };

		if (Input::IsKeyPressed(keys.moveRight))
			moveDir += rightDir;
		if (Input::IsKeyPressed(keys.moveLeft))
			moveDir -= rightDir;
		if (Input::IsKeyPressed((KeyCode)'Q'))
			moveDir += forwardDir;
		if (Input::IsKeyPressed((KeyCode)'E'))
			moveDir -= forwardDir;
		if (Input::IsKeyPressed(keys.moveForward))
			moveDir += upDir;
		if (Input::IsKeyPressed(keys.moveBack))
			moveDir -= upDir;

		int window_border = 20;

		if (VecIsZero(moveDir))
			gameObject.GetComponent<Transform>().translation += moveSpeed * dt * glm::normalize(moveDir);
	}

}