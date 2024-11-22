#pragma once
#include "Maths.h"
#include "vector"

namespace dae
{
	struct Vertex
	{
		Vector3 position{};
		ColorRGB color{colors::White};
		Vector2 uv{}; //W2
		//Vector3 normal{}; //W4
		//Vector3 tangent{}; //W4
		//Vector3 viewDirection{}; //W4
	};

	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		//Vector3 normal{};
		//Vector3 tangent{};
		//Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		Mesh(std::vector<Vertex>&& tVertices, std::vector<uint32_t>&& indices, const PrimitiveTopology primitive_topology, const Matrix& world_matrix):
			vertices(std::move(tVertices)),
			indices(std::move(indices)),
			primitiveTopology(primitive_topology),
			worldMatrix(world_matrix),
			vertices_out(std::vector<Vertex_Out>(vertices.size()))
		{
		}
		
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };
		Matrix worldMatrix{};
		
		std::vector<Vertex_Out> vertices_out{};
	};
}
