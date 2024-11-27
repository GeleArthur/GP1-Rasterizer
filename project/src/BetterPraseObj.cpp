#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#include "BetterPraseObj.h"

#include <algorithm>
#include <iostream>

#include "DataTypes.h"
void dae::Utils::BetterParseObj(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(filename))
	{
		throw std::runtime_error("Can't parse or find obj");
	}


	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			Vertex oneOfThem;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
				tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
				tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
				oneOfThem.position = {vx, vy, vz};
				indices.push_back(idx.vertex_index);

				// Check if `normal_index` is zero or positive. negative = no normal data
				// if (idx.normal_index >= 0) {
				// 	tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
				// 	tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
				// 	tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
				// }

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
					tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
					oneOfThem.uv = {tx, ty};
				}

				// Optional: vertex colors
				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
			}
			index_offset += fv;

			vertices.push_back(oneOfThem);

			// per-face material
			// shapes[s].mesh.material_ids[f];
		}
	}
}
