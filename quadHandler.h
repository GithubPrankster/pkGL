#ifndef QUADHANDLER_H
#define QUADHANDLER_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include <stdexcept>
#include "screenVertex.h"

class Quad {
public:
	Quad() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		
	}
	//Render mesh.
	void render() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	//Dispose of VAO and VBO.
	void terminator() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
private:
	unsigned int VBO, VAO;
	screenVertex vertices[6] = {
	  glm::vec2(-1.0f,  1.0f),  glm::vec2(0.0f, 1.0f),
	  glm::vec2(-1.0f, -1.0f),  glm::vec2(0.0f, 0.0f),
	  glm::vec2(1.0f, -1.0f),  glm::vec2(1.0f, 0.0f),

	  glm::vec2(-1.0f,  1.0f),  glm::vec2(0.0f, 1.0f),
	  glm::vec2(1.0f, -1.0f),  glm::vec2(1.0f, 0.0f),
	  glm::vec2(1.0f,  1.0f),  glm::vec2(1.0f, 1.0f)
	};
};
#endif
