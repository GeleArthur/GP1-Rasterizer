#pragma once
#include "Maths.h"
#include "vector"
#include "Vector.h"

namespace dae
{
	struct Vertex
	{
		Vector<3,float> position{};
		ColorRGB color{colors::White};
		Vector<2,float> uv{}; //W2
		Vector<3,float> normal{}; //W4
		Vector<3,float> tangent{}; //W4
		Vector<3,float> viewDirection{}; //W4
	};

	struct Vertex_Out
	{
		Vector<4,float> position{};
		ColorRGB color{ colors::White };
		Vector<2,float> uv{};
		Vector<3,float> normal{};
		Vector<3,float> tangent{};
		Vector<3,float> viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		Mesh(std::vector<Vertex>&& tVertices, std::vector<uint32_t>&& indices, const PrimitiveTopology primitive_topology, const Matrix<float>& world_matrix):
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
		Matrix<float> worldMatrix{};
		
		std::vector<Vertex_Out> vertices_out{};
		std::vector<Vertex_Out> vertices_clipped{};
	};
}
