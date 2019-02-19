#ifndef TEXHANDLER_H
#define TEXHANDLER_H

#include <glad/glad.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



class textureHandler {
public:
	unsigned int texture;
	textureHandler(const char* name, GLenum rgbType, int scrWidth = 0, int scrHeight = 0) {
		stbi_set_flip_vertically_on_load(true);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Near Filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, rgbType, width, height, 0, rgbType, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "The textureHandler did not appreciate this data screw-ery." << std::endl;
		}
		stbi_image_free(data);
	}
};

#endif
//Leave as 0 if no texture.
void textureBind(unsigned int diffuse, unsigned int specular = 0, unsigned int depthMap = 0) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}
//Create a cool icon and assign it.
void iconAssign(const char* path, GLFWwindow* window) {
	if (stbi_set_flip_vertically_on_load != 0) {
		stbi_set_flip_vertically_on_load(true);
	}
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	GLFWimage img;
	img.width = width, img.height = height, img.pixels = data;
	stbi_image_free(data);
	glfwSetWindowIcon(window, 1, &img);
}
