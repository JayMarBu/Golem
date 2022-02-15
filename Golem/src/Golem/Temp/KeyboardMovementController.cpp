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

	void KeyboardMovementController::MoveInPlaneXZ(float dt, TempGameObject& gameObject, glm::vec2 viewCentre)
	{
		GOL_PROFILE_FUNCTION();

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
				glm::vec3 rot = gameObject.transform.EulerAngles() + turnSpeed * dt * glm::normalize(rotate);
				gameObject.transform.Rotation(rot);
			}
			
		
			gameObject.transform.RotationX(glm::clamp(gameObject.transform.EulerAngles().x, -1.5f, 1.5f));
			gameObject.transform.RotationY(glm::mod(gameObject.transform.EulerAngles().y, glm::two_pi<float>()));

		}

		GOL_PROFILE_SCOPE("KeyboardMovementController::MoveInPlaneXZ -> translation");

		float yaw = gameObject.transform.EulerAngles().y;
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
			gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}

	void KeyboardMovementController::MoveInPlaneXY(float dt, TempGameObject& gameObject)
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
			gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}

	void KeyboardMovementController::DrawGui(TempGameObject& gameObject)
	{
		if (ImGui::CollapsingHeader("Camera Options"))
		{
			ImGui::Indent(10);

			static bool showControls = false;
			ImGui::Checkbox("show Controls##bool", &showControls);
			if (showControls)
			{
				if (two_D)
				{
					ImGui::Text("Camera Controls:");
					ImGui::Text("\tWASD to move.");
					ImGui::Text("\tHold mouse to edge of window to pan");
					ImGui::Text("\tE to zoom out");
					ImGui::Text("\tQ to zoom in");
				}
				else
				{
					ImGui::Text("Camera Controls:");
					ImGui::Text("\tWASD to move.");
					ImGui::Text("\tRight click and drag to turn");
					ImGui::Text("\tSpace to ascend");
					ImGui::Text("\tQ to descend");
				}
			}

			if (ImGui::Checkbox("2d camera controls##bool", &two_D))
			{
				if (two_D == true)
				{
					gameObject.transform.Rotation( 0,0,0 );
				}
			}
			ImGui::Text(("Position: (" + std::to_string(gameObject.transform.translation.x) + ", " + std::to_string(gameObject.transform.translation.y) + ", " + std::to_string(gameObject.transform.translation.z) + ")").c_str());
			ImGui::Text(("rotation: (" + std::to_string(gameObject.transform.rotation.x) + ", " + std::to_string(gameObject.transform.rotation.y) + ", " + std::to_string(gameObject.transform.rotation.z) + ")").c_str());
			ImGui::SliderFloat("Camera move speed", &moveSpeed, 0.1f, 10.0f);
			ImGui::SliderFloat("Camera rotation speed", &turnSpeed, 1.0f, 20.0f);
			ImGui::Indent(-10);
		}
	}

}