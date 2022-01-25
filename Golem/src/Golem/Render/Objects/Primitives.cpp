#include "golpch.h"
#include "Primitives.h"

#define COLOUR_WHITE {1.0f,1.0f,1.0f}
#define NORMAL(x,y,z) glm::normalize(glm::vec3(x,y,z))

namespace golem
{
	namespace Primitives
	{
		void Cube(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 colour)
		{
			const float u = 0.5f;

			// indices cube
			if (indices)
			{
				//TODO: figure out tex coords
				*vertices = {
				{{-u, -u, -u},	COLOUR_WHITE, NORMAL(-1,-1,-1),	{0,0}}, // 0
				{{u, -u, -u},	COLOUR_WHITE, NORMAL(1,-1,-1),	{0,0}}, // 1
				{{-u, u, -u},	COLOUR_WHITE, NORMAL(-1,1,-1),	{0,0}}, // 2
				{{u, u, -u},	COLOUR_WHITE, NORMAL(1,1,-1),	{0,0}}, // 3
				{{-u, -u, u},	COLOUR_WHITE, NORMAL(-1,-1,1),	{0,0}}, // 4
				{{u, -u, u},	COLOUR_WHITE, NORMAL(1,-1,1),	{0,0}}, // 5
				{{-u, u, u},	COLOUR_WHITE, NORMAL(-1,1,1),	{0,0}}, // 6
				{{u, u, u},		COLOUR_WHITE, NORMAL(1,1,1),	{0,0}}  // 7
				};

				*indices = {
					// front face
					0,3,2,
					0,1,3,

					// bottom face
					2,7,6,
					2,3,7,

					// back face
					6,5,4,
					6,7,5,

					// top face
					4,1,0,
					4,5,1,

					// left face
					4,2,6,
					4,0,2,

					// right face
					1,7,3,
					1,5,7
				};
			}

			// texture friendly cube
			else
			{
				*vertices = {
	#pragma region  // ****** Top face ******
					// 1
					// |\
					// 3-2
					{{-u, -u, u},	COLOUR_WHITE, NORMAL(0,-1,0),	{0,0}},
					{{u, -u, -u},	COLOUR_WHITE, NORMAL(0,-1,0),	{1,1}},
					{{-u, -u, -u},	COLOUR_WHITE, NORMAL(0,-1,0),	{0,1}},

					// 1-2 
					//  \|
					//   3
					{{-u, -u, u},	COLOUR_WHITE, NORMAL(0,-1,0),	{0,0}},
					{{u, -u, u},	COLOUR_WHITE, NORMAL(0,-1,0),	{1,0}},
					{{u, -u, -u},	COLOUR_WHITE, NORMAL(0,-1,0),	{1,1}},
	#pragma endregion

	#pragma region  // ****** Front face ******
					// 1
					// |\
					// 3-2
					{{-u, -u, -u},	COLOUR_WHITE, NORMAL(0,0,1),	{0,0}},
					{{u, u, -u},	COLOUR_WHITE, NORMAL(0,0,1),	{1,1}},
					{{-u, u, -u},	COLOUR_WHITE, NORMAL(0,0,1),	{0,1}},

					// 1-2 
					//  \|
					//   3
					{{-u, -u, -u},	COLOUR_WHITE, NORMAL(0,0,1),	{0,0}},
					{{u, -u, -u},	COLOUR_WHITE, NORMAL(0,0,1),	{1,0}},
					{{u, u, -u},	COLOUR_WHITE, NORMAL(0,0,1),	{1,1}},
	#pragma endregion

	#pragma region  // ****** Bottom face ******
					// 1
					// |\
					// 3-2
					{{-u, u, -u},	COLOUR_WHITE, NORMAL(0,1,0),	{0,0}},
					{{u, u, u},		COLOUR_WHITE, NORMAL(0,1,0),	{1,1}},
					{{-u, u, u},	COLOUR_WHITE, NORMAL(0,1,0),	{0,1}},

					// 1-2 
					//  \|
					//   3
					{{-u, u, -u},	COLOUR_WHITE, NORMAL(0,1,0),	{0,0}},
					{{u, u, -u},	COLOUR_WHITE, NORMAL(0,1,0),	{1,0}},
					{{u, u, u},		COLOUR_WHITE, NORMAL(0,1,0),	{1,1}},
	#pragma endregion

	#pragma region  // ****** Back face ******
					//   1
					//  /|
					// 2-3
					{{-u, -u, u},	COLOUR_WHITE, NORMAL(0,0,1),	{1,0}},
					{{u, u, u},		COLOUR_WHITE, NORMAL(0,0,1),	{0,1}},
					{{-u, u, u},	COLOUR_WHITE, NORMAL(0,0,1),	{1,1}},

					// 3-1 
					// |/
					// 2
					{{-u, -u, u},	COLOUR_WHITE, NORMAL(0,0,1),	{1,0}},
					{{u, -u, u},	COLOUR_WHITE, NORMAL(0,0,1),	{0,0}},
					{{u, u, u},		COLOUR_WHITE, NORMAL(0,0,1),	{0,1}},
	#pragma endregion

	#pragma region  // ****** Left face ******
					// 1
					// |\
					// 3-2
					{{-u, -u, u},	COLOUR_WHITE, NORMAL(-1,0,0),	{0,0}},
					{{-u, u, -u},	COLOUR_WHITE, NORMAL(-1,0,0),	{1,1}},
					{{-u, u, u},	COLOUR_WHITE, NORMAL(-1,0,0),	{0,1}},

					// 1-2 
					//  \|
					//   3
					{{-u, -u, u},	COLOUR_WHITE, NORMAL(-1,0,0),	{0,0}},
					{{-u, -u, -u},	COLOUR_WHITE, NORMAL(-1,0,0),	{1,0}},
					{{-u, u, -u},	COLOUR_WHITE, NORMAL(-1,0,0),	{1,1}},
	#pragma endregion

	#pragma region  // ****** Right face ******
					// 1
					// |\
					// 3-2
					{{u, -u, -u},	COLOUR_WHITE, NORMAL(1,0,0),	{0,0}},
					{{u, u, u},		COLOUR_WHITE, NORMAL(1,0,0),	{1,1}},
					{{u, u, -u},	COLOUR_WHITE, NORMAL(1,0,0),	{0,1}},

					// 1-2 
					//  \|
					//   3
					{{u, -u, -u},	COLOUR_WHITE, NORMAL(1,0,0),	{0,0}},
					{{u, -u, u},	COLOUR_WHITE, NORMAL(1,0,0),	{1,0}},
					{{u, u, u},		COLOUR_WHITE, NORMAL(1,0,0),	{1,1}},
	#pragma endregion
				};
			}

		}

		void Quad(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 colour /*= { 1,1,1 }*/)
		{
			const float u = 0.5f;

			if (indices)
			{
				*vertices = {
				{{-u, 0.0, -u},	COLOUR_WHITE, NORMAL(0,-1,0),	{0,0}}, // 0
				{{-u, 0.0, u},	COLOUR_WHITE, NORMAL(0,-1,0),	{0,1}}, // 1
				{{u, 0.0, u},	COLOUR_WHITE, NORMAL(0,-1,0),	{1,1}}, // 2
				{{u, 0.0, -u},	COLOUR_WHITE, NORMAL(0,-1,0),	{1,0}}  // 3
				};

				*indices = {
					0,2,1,
					2,0,3,
				};

				return;
			}

			GOL_CORE_ASSERT(false, "No support for non-indexed quad exists");
			
		}

	}
}