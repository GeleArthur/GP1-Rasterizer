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
		Mesh(std::vector<Vertex>&& tVertices, std::vector<uint32_t>&& _indices, const PrimitiveTopology primitive_topology, const Matrix<float>& world_matrix):
			worldMatrix(world_matrix)
		{
			// We copy over triangleStrip because its easier with multi threading
			if (primitive_topology == PrimitiveTopology::TriangleStrip)
			{
				for (uint32_t i = 0; i < _indices.size() - 2; i+=3)
				{
					Vertex* vertex0;
					Vertex* vertex1;
					Vertex* vertex2;
					
					if (!(i & 1)) // isEven
					{
						vertex0 = &tVertices[_indices[i]];
						vertex1 = &tVertices[_indices[i + 1]];
						vertex2 = &tVertices[_indices[i + 2]];
						
						indices.push_back(_indices[i]);
						indices.push_back(_indices[i+1]);
						indices.push_back(_indices[i+2]);
					}
					else // odd
					{
						vertex0 = &tVertices[_indices[i]];
						vertex2 = &tVertices[_indices[i + 1]];
						vertex1 = &tVertices[_indices[i + 2]];

						indices.push_back(_indices[i]);
						indices.push_back(_indices[i+2]);
						indices.push_back(_indices[i+1]);
					}
					
					vertices.push_back(*vertex0);
					vertices.push_back(*vertex1);
					vertices.push_back(*vertex2);
				}
			}
			else
			{
				vertices = (std::move(tVertices));
				indices = (std::move(_indices));
			}

			vertices_out = (std::vector<Vertex_Out>(vertices.size()));
		}
		
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		// PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };
		Matrix<float> worldMatrix{};
		
		
		std::vector<Vertex_Out> vertices_out{};
		std::vector<Vertex_Out> vertices_clipped{};
	};
}
