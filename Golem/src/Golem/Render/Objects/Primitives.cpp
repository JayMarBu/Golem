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

			enum POS : int
			{
				R_U_B = 0,
				R_D_B = 1,
				R_U_F = 2,
				R_D_F = 3,
				L_U_B = 4,
				L_D_B = 5,
				L_U_F = 6,
				L_D_F = 7,
			};

			glm::vec3 pos[]
			{
				{ u, -u,  u},
				{ u,  u,  u},
				{ u, -u, -u},
				{ u,  u, -u},
				{-u, -u,  u},
				{-u,  u,  u},
				{-u, -u, -u},
				{-u,  u, -u}
			};

			enum UVS : int
			{
				TL = 0,
				TR = 1,
				BL = 2,
				BR = 3
			};

			glm::vec2 uv[]
			{
				{0, 0}, {1, 0},
				{0, 1}, {1, 1}
			};

			enum NORMALS : int
			{
				U	= 0,
				F	= 1,
				L	= 2,
				D	= 3,
				R	= 4,
				B	= 5
			};

			glm::vec3 norm[]
			{
				{ 0.0, -1.0,  0.0},
				{ 0.0,  0.0, -1.0},
				{-1.0,  0.0,  0.0},
				{ 0.0,  1.0,  0.0},
				{ 1.0,  0.0,  0.0},
				{ 0.0,  0.0,  1.0}
			};

			*vertices =
			{
				// Up face
				{pos[L_U_B], COLOUR_WHITE, norm[U], uv[TL]},
				{pos[R_U_F], COLOUR_WHITE, norm[U], uv[BR]},
				{pos[R_U_B], COLOUR_WHITE, norm[U], uv[TR]},

				{pos[L_U_B], COLOUR_WHITE, norm[U], uv[TL]},
				{pos[L_U_F], COLOUR_WHITE, norm[U], uv[BL]},
				{pos[R_U_F], COLOUR_WHITE, norm[U], uv[BR]},

				// Front face
				{pos[R_U_F], COLOUR_WHITE, norm[F], uv[TR]},
				{pos[L_D_F], COLOUR_WHITE, norm[F], uv[BL]},
				{pos[R_D_F], COLOUR_WHITE, norm[F], uv[BR]},

				{pos[L_U_F], COLOUR_WHITE, norm[F], uv[TL]},
				{pos[L_D_F], COLOUR_WHITE, norm[F], uv[BL]},
				{pos[R_U_F], COLOUR_WHITE, norm[F], uv[TR]},

				// Down face
				{pos[L_D_F], COLOUR_WHITE, norm[D], uv[TL]},
				{pos[L_D_B], COLOUR_WHITE, norm[D], uv[BL]},
				{pos[R_D_F], COLOUR_WHITE, norm[D], uv[TR]},

				{pos[R_D_F], COLOUR_WHITE, norm[D], uv[TR]},
				{pos[L_D_B], COLOUR_WHITE, norm[D], uv[BL]},
				{pos[R_D_B], COLOUR_WHITE, norm[D], uv[BR]},

				// Back face
				{pos[L_U_B], COLOUR_WHITE, norm[B], uv[TR]},
				{pos[R_D_B], COLOUR_WHITE, norm[B], uv[BL]},
				{pos[L_D_B], COLOUR_WHITE, norm[B], uv[BR]},

				{pos[R_U_B], COLOUR_WHITE, norm[B], uv[TL]},
				{pos[R_D_B], COLOUR_WHITE, norm[B], uv[BL]},
				{pos[L_U_B], COLOUR_WHITE, norm[B], uv[TR]},

				// Left face
				{pos[L_U_B], COLOUR_WHITE, norm[L], uv[TL]},
				{pos[L_D_B], COLOUR_WHITE, norm[L], uv[BL]},
				{pos[L_D_F], COLOUR_WHITE, norm[L], uv[BR]},

				{pos[L_U_B], COLOUR_WHITE, norm[L], uv[TL]},
				{pos[L_D_F], COLOUR_WHITE, norm[L], uv[BR]},
				{pos[L_U_F], COLOUR_WHITE, norm[L], uv[TR]},

				// Right face
				{pos[R_U_F], COLOUR_WHITE, norm[R], uv[TL]},
				{pos[R_D_F], COLOUR_WHITE, norm[R], uv[BL]},
				{pos[R_D_B], COLOUR_WHITE, norm[R], uv[BR]},

				{pos[R_U_F], COLOUR_WHITE, norm[R], uv[TL]},
				{pos[R_D_B], COLOUR_WHITE, norm[R], uv[BR]},
				{pos[R_U_B], COLOUR_WHITE, norm[R], uv[TR]},
					
			};
		}

		void Quad(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 colour /*= { 1,1,1 }*/)
		{
			const float u = 0.5f;

			if (indices)
			{
				enum UV : int
				{
					TL = 0,
					TR = 1,
					BL = 2,
					BR = 3
				};

				glm::vec2 uv[]
				{
					{0, 0}, {1, 0},
					{0, 1}, {1, 1}
				};

				glm::vec3 pos[]
				{
					{-u, 0,  u},
					{ u, 0,  u},
					{-u, 0, -u},
					{ u, 0, -u}
				};

				*vertices = {
				{pos[TL],	COLOUR_WHITE, NORMAL(0,-1,0),	uv[TL]}, // 0
				{pos[TR],	COLOUR_WHITE, NORMAL(0,-1,0),	uv[TR]}, // 1
				{pos[BL],	COLOUR_WHITE, NORMAL(0,-1,0),	uv[BL]}, // 2
				{pos[BR],	COLOUR_WHITE, NORMAL(0,-1,0),	uv[BR]}  // 3
				};

				*indices = {
					TL,BL,TR,
					TR,BL,BR,
				};

				return;
			}

			GOL_CORE_ASSERT(false, "No support for non-indexed quad exists");
			
		}

	}
}