#pragma once
#include "Vertex.h"
#include "Primitives.h"
#include "Golem/Render/Buffer.h"

namespace golem
{
	class Model
	{
		// Members ********************************************************************************
	public:
		
		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
			void LoadModel(PrimitiveFunc primative, bool useIndices = false, glm::vec3 colour = { 1,1,1 });
		};

	private:
		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount;
		// Methods ********************************************************************************
	public:
		Model(const Builder& builder);
		~Model();

		REMOVE_COPY_CONSTRUCTOR(Model);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		static Ref<Model> CreateModelFromFile(Device& device, const std::string& filepath);
		static Ref<Model> CreateModelFromPrimative(Device& device, PrimitiveFunc primative, bool useIndex = true, glm::vec3 colour = { 1,1,1 });

	private:

		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);
	};
}