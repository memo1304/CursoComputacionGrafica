//Practica10
//Guillermo Espinoza Sanchez
//Fecha de entrega: 22 de octubre de 2026
//319205459

#include <iostream>
#include <cmath>
#include <algorithm>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
 glm::vec3(0.0f,2.0f, 0.0f),
 glm::vec3(0.0f,0.0f, 0.0f),
 glm::vec3(0.0f,0.0f, 0.0f),
 glm::vec3(0.0f,0.0f, 0.0f)
};

float vertices[] = {
 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

 -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

 -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

 -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
 -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
 -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
 -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

glm::vec3 Light1 = glm::vec3(0);

//Anim
float rotBall = 0;
bool AnimBall = false;

// VARIABLES DE LA PISTA CIRCULAR
float dogAngle = 0.0f;
float ballAngle = 3.14159f;
float pathRadius = 2.0f;
float ballPosY = 5.0f;
float dogPosY = 0.0f;

// CONFIGURACIÓN DEL FRENTE DEL PERRO
float dogFacingOffset = 0.0f;
float ballHeadOffset = 0.15f;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Animacion basica - Guillermo Espinoza Sanchez", nullptr, nullptr);

	if (nullptr == window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	Model Dog((char*)"Models/RedDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();
		Animation();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		glm::mat4 view;
		view = camera.GetViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		// --- MODELO DEL PERRO ---
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(pathRadius * cos(dogAngle), dogPosY, pathRadius * sin(dogAngle)));
		model = glm::rotate(model, -dogAngle + glm::radians(dogFacingOffset), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Dog.Draw(lightingShader);

		// --- MODELO DE LA PELOTA ---
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(pathRadius * cos(ballAngle), ballPosY, pathRadius * sin(ballAngle)));
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(1.0f, 0.0f, 1.0f));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);
		glBindVertexArray(0);

		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

	if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.01f;
	if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.01f;
	if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.01f;
	if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.01f;
	if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.1f;
	if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.01f;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active) Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
		else Light1 = glm::vec3(0);
	}
	if (keys[GLFW_KEY_N])
	{
		AnimBall = !AnimBall;
	}
}

void Animation() {
	if (AnimBall) {
		float dogSpeed = 2.1f;
		float ballSpeed = 1.0f;

		dogAngle += dogSpeed * deltaTime;
		ballAngle -= ballSpeed * deltaTime;

		if (dogAngle > 6.2831853f) dogAngle -= 6.2831853f;
		if (ballAngle < 0.0f) ballAngle += 6.2831853f;

		float effectiveDogAngle = dogAngle + 0.18f;

		float diff = effectiveDogAngle - ballAngle;
		if (diff < 0.0f) diff += 6.2831853f * 10.0f;

		float p = fmod(diff, 6.2831853f) / 6.2831853f;

		float distToImpact = std::min(p, 1.0f - p);

		float maxJump = 0.6f;
		if (distToImpact < 0.08f) {
			dogPosY = maxJump * cos((distToImpact / 0.08f) * 1.570796f);
		}
		else {
			dogPosY = 0.0f;
		}

		float baseBallHeight = maxJump + ballHeadOffset;
		float maxBallHeight = 5.0f;
		float bounceAmplitude = maxBallHeight - baseBallHeight;

		ballPosY = baseBallHeight + bounceAmplitude * std::abs(sin(p * 3.14159f));

	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

