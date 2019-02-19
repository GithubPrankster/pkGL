#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
//OpenGL
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
//Custom
#include "shaderLoader.h"
#include "textureHandler.h"
#include "meshHandler.h"
#include "quadHandler.h"
#include "framebufferHandler.h"
#include "cubemapHandler.h"

const int WIDTH = 1600, HEIGHT = 900;

glm::mat4 proj = glm::mat4(1.0f), model = glm::mat4(1.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view;

void initialize(int w, int h);
void inputHandle(GLFWwindow* window, float delta);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

double lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;

float deltaTime = 0.0f, lastFrame = 0.0f;

GLFWwindow* window;

int main() {
	std::cout << "Loading" << std::endl;

	//OpenGL (GLFW/GLAD) Initialization//
	initialize(WIDTH, HEIGHT);
	//Shader Setup
	Shader basic("shaderRes/basic.vs", "shaderRes/basic.fs");
	Shader shadowShader("shaderRes/shadow.vs", "shaderRes/shadow.fs");
	Shader cubemap("shaderRes/cubemap.vs", "shaderRes/cubemap.fs"); 

	//Camera Stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	glfwSetCursorPos(window, lastX, lastY);
	// Material Stuff //
	basic.use();
	basic.setInt("diffuse", 0);
	basic.setInt("specular", 1);
	basic.setInt("depth", 2);

	cubemap.use();
	cubemap.setInt("skybox", 0);

	unsigned int framebuffer, depthAttachment;
	glGenFramebuffers(1, &framebuffer);

	glGenTextures(1, &depthAttachment);
	glBindTexture(GL_TEXTURE_2D, depthAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 lightPos(0.0f, -0.7f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		//Delta time calculation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		inputHandle(window, deltaTime);

		glClearColor(0.39f, 0.128f, 0.152f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightPos.y = 1.0 + cos(glfwGetTime()) * 1.0f;

		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		

		float near_plane = 0.1f, far_plane = 15.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		glm::mat4 lightView = glm::lookAt(lightPos,
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.39f, 0.128f, 0.152f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic.use();

		basic.setVec3("light.position", lightPos);
		basic.setVec3("viewPos", cameraPos);
		basic.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		basic.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);

		basic.setMat4("model", model);
		basic.setMat4("view", view);
		basic.setMat4("proj", proj);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}

void initialize(int w, int h) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(w, h, "pkGL Ver 0.2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "GLFW has decided to game end itself." << std::endl;
		glfwTerminate();
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void inputHandle(GLFWwindow* window, float delta) {
	float velocity = 4.0f * delta;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += velocity * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= velocity * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= velocity * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += velocity * glm::normalize(glm::cross(cameraFront, cameraUp));

	cameraPos.y = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos, lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.08;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}