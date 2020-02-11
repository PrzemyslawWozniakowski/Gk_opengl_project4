#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <stb_image.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "Camera.h"

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void setUpShader(Camera camera, Shader shader, float ambientStrength, glm::vec3 pointLightsPosition[], glm::vec3 spotLightPositions[], glm::vec3 spotLightDirs[], bool addFog, bool usePhong, bool offLights, bool flashlight);
#define PI 3.14159265359

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 1.5f, 3.0f));
Camera movingCamera(glm::vec3(0.0f, 1.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int cameraType = 0;
bool usePhong = true;
bool usePhongShading = true;
float lastClicked = 0.0f;
bool changeDay = false;
bool addFog = false;
bool showPointLights = true;
bool offLights = false;
bool flashlight = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(1200, 900, "Gk Projekt 4", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader phongShader("Source/phongshader.vs", "Source/phongshader.fs");
	Shader lampShader("Source/lamp.vs", "Source/lamp.fs");
	Shader floorShader("Source/floorshader.vs", "Source/floorshader.fs");
	Shader gourardShader("Source/gourardshader.vs", "Source/gourardshader.fs");


	float floor[] = {
		 0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
	};

	int floorindices[] = {
		0,1,2,
		1,2,3
	};

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

#pragma region Sphere


	std::vector<float> svertices;
	std::vector<int> indices;

	float radius = 1.0f;
	int sectorCount = 30, stackCount = 30;
	float x, y, z, xy;                              
	float nx, ny, nz, lengthInv = 1.0f / radius;    

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;       
		xy = radius * cosf(stackAngle);          
		z = radius * sinf(stackAngle);             

		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;          

			x = xy * cosf(sectorAngle);           
			y = xy * sinf(sectorAngle);          
			svertices.push_back(x);
			svertices.push_back(y);
			svertices.push_back(z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			svertices.push_back(nx);
			svertices.push_back(ny);
			svertices.push_back(nz);
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);  
		k2 = k1 + sectorCount + 1;    

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

#pragma endregion
#pragma region Bind arrays

	unsigned int fVBO, fVAO, fEBO;
	glGenVertexArrays(1, &fVAO);
	glGenBuffers(1, &fVBO);
	glGenBuffers(1, &fEBO);

	glBindVertexArray(fVAO);

	glBindBuffer(GL_ARRAY_BUFFER, fVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor), floor, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorindices), floorindices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int sphereVBO, sphereVAO, EBO;
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(sphereVAO);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, svertices.size() * sizeof(float), svertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



#pragma endregion

#pragma region Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	unsigned char *data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
#pragma endregion

	glm::vec3 pointLightPositions[] = {
	  glm::vec3(0.7f,  0.2f,  2.0f),
	  glm::vec3(2.3f, -3.3f, -4.0f),
	  glm::vec3(-4.0f,  2.0f, -12.0f),
	  glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec3 spotLightPositions[] = {
	  glm::vec3(0.0f,0.0f,0.0f),
	  glm::vec3(-5.0f,4.0f,-5.0f),
	};

	glm::vec3 spotLightDirs[] = {
	  glm::vec3(0.0f,0.0f,0.0f),
	  glm::normalize( glm::vec3(0.0f,-1.0f,0.1f)),
	};

	float movex = 0.01f;
	float movez = 0.00f;

	float xpos = 0.0f;
	float zpos = 0.0f;
	float ambientChange = -0.0001f;
	float ambientStrength = 0.2f;

	float ypos = 0;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		if (addFog)
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		else
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader shader = phongShader;
		if (!usePhongShading)
			shader = gourardShader;

		glBindVertexArray(cubeVAO);
		glm::vec3 ballPosition = glm::vec3(7.5 + 2 * cos((float)glfwGetTime()), -1.0, 3.5 + 2 * sin((float)glfwGetTime()));
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view;

		glm::vec3 movingContainer = glm::vec3(0 + xpos, -2.5, 0 + zpos);
		glm::vec3 movingCameraPos = glm::vec3(movingContainer.x - 5.0, movingContainer.y + 4.0, movingContainer.z + 1.5f);
		xpos += movex;
		zpos += movez;
		if (xpos > 6.0f)
		{
			xpos = 6.0f;
			movex = 0.0f;
			movez = 0.01f;
		}
		if (zpos > 6.0f)
		{
			zpos = 6.0f;
			movez = 0.0f;
			movex = -0.01f;
		}
		if (xpos < -6.0f)
		{
			xpos = -6.0f;
			movex = 0.0f;
			movez = -0.01f;
		}
		if (zpos < -6.0f)
		{
			zpos = -6.0f;
			movez = 0.0f;
			movex = 0.01f;
		}
		spotLightPositions[0] = movingContainer;
		spotLightDirs[0] = glm::normalize( glm::vec3(movex * 100, -0.35f, movez * 100));

		if (cameraType == 0)
			view = camera.GetViewMatrix();
		if (cameraType == 1)
			view = camera.GetLookAt(ballPosition);
		if (cameraType == 2)
		{
			movingCamera.Position = movingCameraPos;
			view = movingCamera.GetLookAt(movingContainer);
		}
		

#pragma region SetShader
		shader.use();
		setUpShader(camera, shader, ambientStrength, pointLightPositions, spotLightPositions, spotLightDirs, addFog, usePhong, offLights, flashlight);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
#pragma endregion

		//============================================================

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 0.9f, 0.5f, 0.7f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 2.0f+ypos/3, 4.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 0.2f, 0.1f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 2.0f+ypos/3, 4.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		ypos += movex;

		model = glm::mat4(1.0f);
		model = glm::translate(model, movingContainer);
		shader.setMat4("model", model);
		model = glm::scale(model, glm::vec3(0.6f));
		shader.setVec3("objectColor", 1.0f, 0.3f, 0.3f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f,1.0f,-10.0f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime() / 2, glm::vec3(0.0f, 0.0f, 1.0f));
		shader.setMat4("model", model);
		model = glm::scale(model, glm::vec3(0.6f));
		shader.setVec3("objectColor", 0.6f, 0.8f, 0.9f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//===========================================
		glBindVertexArray(sphereVAO);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f,3.0f,6.0f));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 1.0f, 1.0f, 0.1f);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, ballPosition);
		shader.setMat4("model", model1);
		shader.setVec3("objectColor", 0.4f, 1.0f, 0.31f);
		model1 = glm::scale(model1, glm::vec3(0.8f));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.0f, -2.0f, 9.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 0.9f, 0.9f, 0.9f);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.0f, -0.5f, 9.0f));
		model = glm::scale(model, glm::vec3(0.6f));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 0.9f, 0.9f, 0.9f);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.0f, 0.25f, 9.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 0.9f, 0.9f, 0.9f);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		//========================================================
		if (showPointLights && !offLights)
		{

			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);

			for (int i = 0; i <4; i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
				lampShader.setMat4("model", model);

				glBindVertexArray(lightVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

		}

		//========================================================
		floorShader.use();
		setUpShader(camera, floorShader, ambientStrength, pointLightPositions, spotLightPositions, spotLightDirs, addFog, usePhong, offLights, flashlight);

		glm::mat4 modelf = glm::mat4(1.0f);
		modelf = glm::translate(modelf, glm::vec3(0, -5.0f, 0));
		modelf = glm::scale(modelf, glm::vec3(25.0f));
		floorShader.setMat4("projection", projection);
		floorShader.setMat4("view", view);
		floorShader.setMat4("model", modelf);

		glBindVertexArray(fVAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		if (changeDay)
		{
			ambientStrength += ambientChange;
			if (ambientStrength <= -0.1f || ambientStrength >= 0.5f)
				ambientChange = -ambientChange;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &fVAO);
	glDeleteBuffers(1, &fVBO);
	glDeleteBuffers(1, &fEBO);
	
	glfwTerminate();
	return 0;
}
//


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if (((float)glfwGetTime() - lastClicked) > 0.5f)
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			cameraType++;
			cameraType = cameraType % 3;
			if (cameraType == 1)
				camera = Camera(glm::vec3(-1.0f, 4.0f, 2.0f));
			if (cameraType == 0)
				camera.Position = glm::vec3(0.0f, 1.5f, 3.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			usePhong = !usePhong;
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			usePhongShading = !usePhongShading;
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
			addFog = !addFog;
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			changeDay = !changeDay;
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			showPointLights = !showPointLights;
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			flashlight = !flashlight;
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			offLights = !offLights;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lastClicked = (float)glfwGetTime();
	if (cameraType == 0)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (cameraType == 0)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; 

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void setUpShader(Camera cam, Shader shader, float ambientStrength, glm::vec3 pointLightPositions[], glm::vec3 spotLightPositions[], glm::vec3 spotLightDirs[] , bool addFog, bool usePhong, bool offLights, bool flashlight)
{
	shader.setVec3("viewPos", camera.Position);
	shader.setBool("usePhong", usePhong);
	shader.setBool("addFog", addFog);
	shader.setFloat("specularStrength", 1.0f);
	shader.setFloat("diffStrength", 1.0f);
	if (changeDay)
	{

		if (ambientStrength >= 0.0f && ambientStrength <= 0.4f)
			shader.setFloat("ambientStrength", ambientStrength);
		else
		{
			if (ambientStrength < 0.0f)
				shader.setFloat("ambientStrength", 0.0f);
			else
				shader.setFloat("ambientStrength", 0.4f);
		}
	}
	else
	{
		shader.setFloat("ambientStrength", 0.2f);
	}

	shader.setFloat("specularStrength", 1.0f);
	shader.setFloat("diffStrength", 1.0f);
	shader.setInt("m", 32);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	if (!offLights)
	{
		// point light 1
		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].lightColor", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09);
		shader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].lightColor", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[1].constant", 1.0f);
		shader.setFloat("pointLights[1].linear", 0.09);
		shader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		shader.setVec3("pointLights[2].position", pointLightPositions[2]);
		shader.setVec3("pointLights[2].lightColor", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[2].constant", 1.0f);
		shader.setFloat("pointLights[2].linear", 0.09);
		shader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		shader.setVec3("pointLights[3].position", pointLightPositions[3]);
		shader.setVec3("pointLights[3].lightColor", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[3].constant", 1.0f);
		shader.setFloat("pointLights[3].linear", 0.09);
		shader.setFloat("pointLights[3].quadratic", 0.032);
	}
	else
	{
		// point light 1
		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].lightColor", 0.0f,0.0f,0.0f);
		shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09);
		shader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].lightColor", 0.0f, 0.0f, 0.0f);
		shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[1].constant", 1.0f);
		shader.setFloat("pointLights[1].linear", 0.09);
		shader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		shader.setVec3("pointLights[2].position", pointLightPositions[2]);
		shader.setVec3("pointLights[2].lightColor", 0.0f, 0.0f, 0.0f);
		shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[2].constant", 1.0f);
		shader.setFloat("pointLights[2].linear", 0.09);
		shader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		shader.setVec3("pointLights[3].position", pointLightPositions[3]);
		shader.setVec3("pointLights[3].lightColor", 0.0f, 0.0f, 0.0f);
		shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[3].constant", 1.0f);
		shader.setFloat("pointLights[3].linear", 0.09);
		shader.setFloat("pointLights[3].quadratic", 0.032);
	}

	shader.setVec3("spotLight[0].position", camera.Position);
	shader.setVec3("spotLight[0].lightColor", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight[0].direction", camera.Front);
	shader.setVec3("spotLight[0].diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight[0].constant", 0.8f);
	shader.setFloat("spotLight[0].linear", 0.09);
	shader.setFloat("spotLight[0].quadratic", 0.032);
	shader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.5f)));
	shader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));

	if (flashlight && cameraType==0)
	{
		shader.setVec3("spotLight[0].lightColor", 1.0f, 1.0f, 1.0f);
	}
	else
	{
		shader.setVec3("spotLight[0].lightColor", 0.0f, 0.0f, 0.0f);
	}
	//
	shader.setVec3("spotLight[1].position",spotLightPositions[0]);
	shader.setVec3("spotLight[1].lightColor", 1.0f, 1.0f, 0.0f);
	shader.setVec3("spotLight[1].direction", spotLightDirs[0]);
	shader.setVec3("spotLight[1].diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight[1].constant", 1.0f);
	shader.setFloat("spotLight[1].linear", 0.09);
	shader.setFloat("spotLight[1].quadratic", 0.032);
	shader.setFloat("spotLight[1].cutOff", glm::cos(glm::radians(10.5f)));
	shader.setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(15.0f)));

	shader.setVec3("spotLight[2].position", spotLightPositions[1]);
	shader.setVec3("spotLight[2].lightColor", 1.0f, 0.2f, 1.0f);
	shader.setVec3("spotLight[2].direction", spotLightDirs[1]);
	shader.setVec3("spotLight[2].diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight[2].constant", 0.8f);
	shader.setFloat("spotLight[2].linear", 0.06f);
	shader.setFloat("spotLight[2].quadratic", 0.016f);
	shader.setFloat("spotLight[2].cutOff", glm::cos(glm::radians(10.5f)));
	shader.setFloat("spotLight[2].outerCutOff", glm::cos(glm::radians(20.0f)));
}