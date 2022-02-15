#pragma once
#include "Golem/Input/KeyCodes.h"
#include "../Window.h"
#include "../ECS/GameObject.h"

namespace golem
{
	class KeyboardMovementController
	{
		// Members ********************************************************************************
	public:

		struct KeyMappings
		{
			KeyCode moveLeft = KeyCode::A;
			KeyCode moveRight = KeyCode::D;
			KeyCode moveForward = KeyCode::W;
			KeyCode moveBack = KeyCode::S;
			KeyCode moveUp = KeyCode::SPACE;
			KeyCode moveDown = KeyCode::Q;

			KeyCode lookLeft = KeyCode::LEFT;
			KeyCode lookRight = KeyCode::RIGHT;
			KeyCode lookUp = KeyCode::UP;
			KeyCode lookDown = KeyCode::DOWN;
		};

		KeyMappings keys{};

		float moveSpeed{ 1.f };
		float turnSpeed{ 5.f };

		float deltax, deltay;
		float displayDeltax, displayDeltay;

		bool two_D = true;
		// Methods ********************************************************************************

	public:

		void MoveInPlaneXZ(float dt, GameObject gameObject, glm::vec2 viewCentre);
		void MoveInPlaneXY(float dt, GameObject gameObject);
	};
}