#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Golem/Utils.h"

namespace golem
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 colour;
		glm::vec3 normals;
		glm::vec2 texcoords;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

		bool operator==(const Vertex& other) const
		{
			return	position == other.position &&
				colour == other.colour &&
				normals == other.normals &&
				texcoords == other.texcoords;
		};
	};
}

namespace std
{
	template<>
	struct hash<golem::Vertex>
	{
		size_t operator()(golem::Vertex const& v) const
		{
			size_t seed = 0;
			golem::HashCombine(seed, v.position, v.colour, v.normals, v.texcoords);
			return seed;
		}
	};
}