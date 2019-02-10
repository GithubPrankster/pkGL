#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "vertex.h"

void loadObj(const char* path, std::vector<vertex>& verty) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path)) {
		throw std::runtime_error(err);
	}
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertex vert = {};

			vert.pos = {
				glm::vec3(attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2])
			};

			vert.normal = {
				glm::vec3(attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2])
			};

			vert.tex = {
				glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1])
			};
			verty.push_back(vert);
		}
	}

}