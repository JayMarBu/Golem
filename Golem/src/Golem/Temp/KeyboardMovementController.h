#pragma once
#include "Golem/Input/KeyCodes.h"
#include "TempGameObject.h"
#include "../Window.h"

namespace golem
{
	class KeyboardMovementController
	{
		// Members ********************************************************************************
	public:

		struct KeyMappings
		{
			int moveLeft = GOL_KEY_A;
			int moveRight = GOL_KEY_D;
			int moveForward = GOL_KEY_W;
			int moveBack = GOL_KEY_S;
			int moveUp = GOL_KEY_SPACE;
			int moveDown = GOL_KEY_Q;

			int lookLeft = GOL_KEY_LEFT;
			int lookRight = GOL_KEY_RIGHT;
			int lookUp = GOL_KEY_UP;
			int lookDown = GOL_KEY_DOWN;
		};

		KeyMappings keys{};

		float moveSpeed{ 1.f };
		float turnSpeed{ 5.f };

		float deltax, deltay;
		float displayDeltax, displayDeltay;

		bool two_D = true;
		// Methods ********************************************************************************

	public:

		void MoveInPlaneXZ(float dt, TempGameObject& gameObject, glm::vec2 viewCentre);
		void MoveInPlaneXY(float dt, TempGameObject& gameObject);

		void DrawGui(TempGameObject& gameObject);
	};
}