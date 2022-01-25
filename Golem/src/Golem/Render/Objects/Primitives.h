#pragma once
#include "Vertex.h"

namespace golem
{
	using PrimitiveFunc = std::function<void(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 colour)>;

	namespace Primitives
	{
		void Cube(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 colour = { 1,1,1 });

		void Quad(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, glm::vec3 colour = { 1,1,1 });
	}
}