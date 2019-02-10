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
#include "cameraHandler.h"
#include "quadHandler.h"
#include "framebufferHandler.h"
#include "shadowcubeHandler.h"

const int WIDTH = 1600, HEIGHT = 900;

enum Camera_Movement {
	FORWARD, BACKWARD, LEFT, RIGHT
};

glm::mat4 proj = glm::mat4(1.0f), model = glm::mat4(1.0f);

void initialize(int w, int h);
void processKeyboard(Camera_Movement direction, float deltaTime, bool fly);
void inputHandle(GLFWwindow* window, Camera camera, float delta);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f, lastFrame = 0.0f;

GLFWwindow* window;

int main() {
	std::cout << "Loading" << std::endl;

	//OpenGL (GLFW/GLAD) Initialization//
	initialize(WIDTH, HEIGHT);
	//Shader Setup
	Shader basic("shaderRes/basic.vs", "shaderRes/basic.fs");
	//Shader shadow("shaderRes/shadow.vs", "shaderRes/shadow.fs"); //woah geometry shasha
	Shader shadowCube("shaderRes/shadowcube.vs", "shaderRes/shadowcube.fs", "shaderRes/shadowcube.gs");
	//Mesh Data
	Quad screenObject;
	//Texture Data
	//Camera Stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	model = glm::translate(glm::vec3(0.0, 0.5, 0.0));
	proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	glfwSetCursorPos(window, lastX, lastY);
	camera.MovementSpeed = 4.0f;
	// Material Stuff //
	basic.use();
	basic.setInt("diffuse", 0);
	basic.setInt("shadowMap", 1);

	shadowcubeHandler shadowHandle(1024);

	glm::vec3 lightPos(1.0f, 1.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		//Delta time calculation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		inputHandle(window, camera, deltaTime);

		glClearColor(0.39f, 0.128f, 0.152f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//shadowBuffer (cubemap ver)
		
		float near_plane = 0.1f, far_plane = 25.0f;

		glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowHandle.bufferID);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (unsigned int i = 0; i < 6; ++i)
			shadowCube.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		shadowCube.setFloat("far_plane", far_plane);
		shadowCube.setVec3("lightPos", lightPos);

		//Render section
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic.use();
		//Light position and view position, light right now is player
		basic.setVec3("light.position", lightPos);
		basic.setVec3("viewPos", camera.Position);

		// Light stuff
		basic.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
		basic.setFloat("far_plane", far_plane);

		glm::mat4 view = camera.GetViewMatrix();

		//Second Render section

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

	window = glfwCreateWindow(w, h, "RDV", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "GLFW has decided to game end itself." << std::endl;
		glfwTerminate();
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processKeyboard(Camera_Movement direction, float deltaTime, bool fly)
{
	float velocity = camera.MovementSpeed * deltaTime;
	if (direction == FORWARD)
		camera.Position += camera.Front * velocity;
	if (direction == BACKWARD)
		camera.Position -= camera.Front * velocity;
	if (direction == LEFT)
		camera.Position -= camera.Right * velocity;
	if (direction == RIGHT)
		camera.Position += camera.Right * velocity;
	if (!fly) {
		camera.Position.y = 0.0f;
	}
}

void inputHandle(GLFWwindow* window, Camera camera, float delta) {
	bool flying = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		flying = true;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		processKeyboard(FORWARD, delta, flying);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		processKeyboard(BACKWARD, delta, flying);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		processKeyboard(LEFT, delta, flying);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		processKeyboard(RIGHT, delta, flying);
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

	double xoffset = xpos - lastX, yoffset = lastY - ypos;
    // reversed since y-coordinates go from bottom to top

	lastX = xpos, lastY = ypos;
	
	camera.ProcessMouseMovement(xoffset, yoffset);
}
