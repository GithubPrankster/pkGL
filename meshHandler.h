#ifndef MESHHANDLER_H
#define MESHHANDLER_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include <stdexcept>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "vertex.h"

class Mesh {
public:
	glm::mat4 mat;
	Mesh(const char* path) {
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
				vertices.push_back(vert);
			}
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	//Render mesh.
	void render() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	//Dispose of VAO and VBO.
	void terminator() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
private:
	unsigned int VBO, VAO;
	std::vector<vertex> vertices;
};
#endif
