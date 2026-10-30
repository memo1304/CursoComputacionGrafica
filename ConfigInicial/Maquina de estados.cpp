//Practica 11
//Guillermo Espinoza Sánchez
//Fecha de entrega: 29 de octubre de 2026
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
#include <vector>

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void ResetDog();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 2.0f, 0.0f);
bool active = false;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
glm::vec3(0.0f, 2.0f, 0.0f),
glm::vec3(0.0f, 0.0f, 0.0f),
glm::vec3(0.0f, 0.0f, 0.0f),
glm::vec3(0.0f, 0.0f, 0.0f)
};

float vertices[] = {
-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

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


glm::vec3 Light1 = glm::vec3(0);

// Anim pelota
float rotBall = 0.0f;
bool  AnimBall = false;

// ============ WAYPOINTS DE LA RUTA ============
const float LIMITE_X = 2.28f;
const float LIMITE_Z = 2.30f;

std::vector<glm::vec3> waypoints = {
glm::vec3(0.0f,    0.0f,  LIMITE_Z),   // A
glm::vec3(LIMITE_X, 0.0f,  LIMITE_Z),   // B
glm::vec3(LIMITE_X, 0.0f, -LIMITE_Z),   // C
glm::vec3(-LIMITE_X, 0.0f, -LIMITE_Z),  // D
glm::vec3(0.0f,    0.0f,  0.0f)         // Home
};
int   currentWaypoint = 0;

const float DOG_SPEED = 0.8f;         // vel. crucero
const float DOG_SPEED_SLOW = 0.35f;   // vel. al acercarse a un waypoint
const float SLOW_RADIUS = 0.9f;       // radio donde empieza a frenar
const float ARRIVE_RADIUS = 0.12f;    // radio para considerar "llegó"

const float MAX_TURN_RATE = 140.0f;   // grados/segundo (limita el giro -> arco)
const float BODY_LEAN_MAX = 8.0f;     // grados de inclinación lateral en curva

// ============ ESTADO DEL PERRO ============
glm::vec3 dogPos(0.0f, 0.0f, 0.0f);
float     dogRot = 0.0f;      // yaw del cuerpo
float     headYaw = 0.0f;     // yaw de la cabeza (mira hacia donde va)
float     bodyLean = 0.0f;    // roll lateral por la curva
float     currentSpeed = 0.0f;

float FLegs = 0.0f, RLegs = 0.0f;
float head = 0.0f, tail = 0.0f;
float walkTimer = 0.0f;
float walkAmount = 0.0f;      // 0..1, cuánto está caminando

bool  routeActive = false;    // false = esperando que se presione B
bool  finalAlign = false;    // true = fase de acomodo exacto en el origen

// ============ DELTA TIME ============
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ============ RESET DEL PERRO ============
void ResetDog() {
	currentWaypoint = 0;
	dogPos = glm::vec3(0.0f);
	dogRot = 0.0f;
	headYaw = 0.0f;
	bodyLean = 0.0f;
	currentSpeed = 0.0f;
	FLegs = RLegs = head = tail = 0.0f;
	walkTimer = 0.0f;
	walkAmount = 0.0f;
	routeActive = false;
	finalAlign = false;
}

int main()
{
	// Init GLFW
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Animacion maquina de estados - Guillermo Espinoza Sanchez", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Models
	Model DogBody((char*)"Models/DogBody.obj");
	Model HeadDog((char*)"Models/HeadDog.obj");
	Model DogTail((char*)"Models/TailDog.obj");
	Model F_RightLeg((char*)"Models/F_RightLegDog.obj");
	Model F_LeftLeg((char*)"Models/F_LeftLegDog.obj");
	Model B_RightLeg((char*)"Models/B_RightLegDog.obj");
	Model B_LeftLeg((char*)"Models/B_LeftLegDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");

	// Container VAO/VBO
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

	glEnable(GL_DEPTH_TEST);

	// Game loop
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

		glm::mat4 modelTemp = glm::mat4(1.0f);

		// Lighting shader
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = std::abs(std::sin(glfwGetTime() * Light1.x));
		lightColor.y = std::abs(std::sin(glfwGetTime() * Light1.y));
		lightColor.z = std::abs(std::sin(glfwGetTime() * Light1.z));

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// SpotLight
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

		glm::mat4 view = camera.GetViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1.0f);

		// Piso
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

		// ============ CUERPO ============
		model = glm::mat4(1.0f);
		model = glm::translate(model, dogPos);
		model = glm::rotate(model, glm::radians(dogRot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(bodyLean), glm::vec3(0.0f, 0.0f, 1.0f));
		modelTemp = model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogBody.Draw(lightingShader);

		// ============ CABEZA ============
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.093f, 0.208f));
		model = glm::rotate(model, glm::radians(headYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(head), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		HeadDog.Draw(lightingShader);

		// ============ COLA ============
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.026f, -0.288f));
		model = glm::rotate(model, glm::radians(tail), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogTail.Draw(lightingShader);

		// ============ PATA DELANTERA IZQUIERDA ============
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.112f, -0.044f, 0.074f));
		model = glm::rotate(model, glm::radians(FLegs), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_LeftLeg.Draw(lightingShader);

		// ============ PATA DELANTERA DERECHA ============
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.111f, -0.055f, 0.074f));
		model = glm::rotate(model, glm::radians(RLegs), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_RightLeg.Draw(lightingShader);

		// ============ PATA TRASERA IZQUIERDA ============
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.082f, -0.046f, -0.218f));
		model = glm::rotate(model, glm::radians(RLegs), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_LeftLeg.Draw(lightingShader);

		// ============ PATA TRASERA DERECHA ============
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.083f, -0.057f, -0.231f));
		model = glm::rotate(model, glm::radians(FLegs), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_RightLeg.Draw(lightingShader);

		// ============ PELOTA ============
		model = glm::mat4(1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);

		glBindVertexArray(0);

		// ============ LAMPARA ============
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
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

// ============ MOVIMIENTO DE CAMARA / LUCES ============
void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.01f;
	if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.01f;
	if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.01f;
	if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.01f;
	if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.01f;
	if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.01f;
}

// ============ CALLBACK DE TECLADO ============
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)   keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	// Solo al presionar (evita multi-disparo)
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_SPACE)
		{
			active = !active;
			Light1 = active ? glm::vec3(0.2f, 0.8f, 1.0f) : glm::vec3(0.0f);
		}
		if (key == GLFW_KEY_N) AnimBall = !AnimBall;

		// B = iniciar la ruta (solo si no está activa)
		if (key == GLFW_KEY_B) {
			if (!routeActive) {
				ResetDog();          // parte desde cero
				routeActive = true;  // arranca la ruta
			}
		}

		// R = reset total (regresa al origen y queda esperando B)
		if (key == GLFW_KEY_R) {
			ResetDog();
		}
	}
}

// ============ ANIMACION / LOCOMOCION NATURAL ============
void Animation()
{
	// Pelota
	if (AnimBall) rotBall += 0.4f;

	const float dt = deltaTime;
	if (dt <= 0.0f) return;

	// ---------- 0) Si la ruta no está activa, solo relajar patas ----------
	if (!routeActive) {
		currentSpeed = 0.0f;
		auto lerpTo0 = [&](float& v, float k) {
			v += (0.0f - v) * std::min(1.0f, k * dt);
			};
		lerpTo0(FLegs, 6.0f);
		lerpTo0(RLegs, 6.0f);
		lerpTo0(head, 6.0f);
		lerpTo0(tail, 6.0f);
		lerpTo0(headYaw, 4.0f);
		lerpTo0(bodyLean, 4.0f);
		walkAmount += (0.0f - walkAmount) * std::min(1.0f, 6.0f * dt);
		return;
	}

	// ---------- 1) Objetivo actual ----------
	bool hasTarget = currentWaypoint < (int)waypoints.size();
	glm::vec3 target = hasTarget ? waypoints[currentWaypoint] : dogPos;

	glm::vec3 to = target - dogPos;
	to.y = 0.0f;
	float distToTarget = glm::length(to);

	// ---------- 2) ¿Llegamos? -> siguiente waypoint ----------
	if (hasTarget && distToTarget < ARRIVE_RADIUS) {
		currentWaypoint++;
		hasTarget = currentWaypoint < (int)waypoints.size();
		if (hasTarget) {
			target = waypoints[currentWaypoint];
			to = target - dogPos; to.y = 0.0f;
			distToTarget = glm::length(to);
		}
	}

	// ---------- 2b) ¿Terminó la ruta? -> fase de acomodo exacto ----------
	if (!hasTarget) {
		finalAlign = true;
	}

	if (finalAlign) {
		// Lerp suave de posición y rotación hacia los valores exactos iniciales
		glm::vec3 homePos(0.0f, 0.0f, 0.0f);
		float homeRot = 0.0f;

		float k = std::min(1.0f, 6.0f * dt);

		dogPos += (homePos - dogPos) * k;

		// rotación con wrap-around para no girar 350° cuando faltan 10°
		float diff = homeRot - dogRot;
		while (diff > 180.0f) diff -= 360.0f;
		while (diff < -180.0f) diff += 360.0f;
		dogRot += diff * k;

		// relajar todo lo demás
		auto lerpTo0 = [&](float& v, float kk) {
			v += (0.0f - v) * std::min(1.0f, kk * dt);
			};
		lerpTo0(headYaw, 6.0f);
		lerpTo0(bodyLean, 6.0f);
		lerpTo0(FLegs, 6.0f);
		lerpTo0(RLegs, 6.0f);
		lerpTo0(head, 6.0f);
		lerpTo0(tail, 6.0f);

		currentSpeed = 0.0f;
		walkAmount += (0.0f - walkAmount) * std::min(1.0f, 6.0f * dt);

		// Cuando esté suficientemente cerca, SNAP exacto y terminar
		if (glm::length(dogPos - homePos) < 0.005f &&
			std::abs(diff) < 0.5f) {
			dogPos = homePos;   // exacto (0,0,0)
			dogRot = homeRot;   // exacto 0°
			headYaw = 0.0f;
			bodyLean = 0.0f;
			FLegs = RLegs = head = tail = 0.0f;
			walkAmount = 0.0f;
			currentSpeed = 0.0f;
			finalAlign = false;
			routeActive = false;   // ahora sí, esperando B o R
		}
		return;
	}

	// ---------- 3) Velocidad deseada (frena suave al acercarse) ----------
	float desiredSpeed = 0.0f;
	if (hasTarget) {
		float t = glm::clamp(distToTarget / SLOW_RADIUS, 0.0f, 1.0f);
		float k = t * t;
		desiredSpeed = DOG_SPEED_SLOW + (DOG_SPEED - DOG_SPEED_SLOW) * k;
	}
	currentSpeed += (desiredSpeed - currentSpeed) * std::min(1.0f, 5.0f * dt);

	// ---------- 4) Dirección deseada y giro limitado -> ARCO ----------
	float yawDelta = 0.0f;
	if (hasTarget && distToTarget > 0.001f) {
		float targetYaw = glm::degrees(std::atan2(to.x, to.z));
		yawDelta = targetYaw - dogRot;
		while (yawDelta > 180.0f) yawDelta -= 360.0f;
		while (yawDelta < -180.0f) yawDelta += 360.0f;

		float maxStep = MAX_TURN_RATE * dt;
		float applied = glm::clamp(yawDelta, -maxStep, maxStep);
		dogRot += applied;
		yawDelta = applied;
	}

	// ---------- 5) Avanzar en la dirección del cuerpo ----------
	if (currentSpeed > 0.001f) {
		glm::vec3 forward(sin(glm::radians(dogRot)), 0.0f, cos(glm::radians(dogRot)));
		float stepDist = currentSpeed * dt;
		if (hasTarget) stepDist = std::min(stepDist, distToTarget);
		dogPos += forward * stepDist;
	}

	// ---------- 6) Inclinación del cuerpo (roll) en curvas ----------
	float turnRate = yawDelta / dt;
	float leanTarget = glm::clamp(turnRate * 0.06f, -BODY_LEAN_MAX, BODY_LEAN_MAX);
	bodyLean += (leanTarget - bodyLean) * std::min(1.0f, 8.0f * dt);

	// ---------- 7) Cabeza: anticipa la curva ----------
	if (hasTarget && distToTarget > 0.001f) {
		float targetYaw = glm::degrees(std::atan2(to.x, to.z));
		float diff = targetYaw - dogRot;
		while (diff > 180.0f) diff -= 360.0f;
		while (diff < -180.0f) diff += 360.0f;
		float headTarget = glm::clamp(diff, -40.0f, 40.0f);
		headYaw += (headTarget - headYaw) * std::min(1.0f, 6.0f * dt);
	}
	else {
		headYaw += (0.0f - headYaw) * std::min(1.0f, 4.0f * dt);
	}

	// ---------- 8) Animación de patas / cola / cabeceo ----------
	float targetWalk = glm::clamp(currentSpeed / DOG_SPEED, 0.0f, 1.0f);
	walkAmount += (targetWalk - walkAmount) * std::min(1.0f, 6.0f * dt);

	if (walkAmount > 0.01f) {
		walkTimer += dt * (6.0f + 6.0f * currentSpeed);

		float s = std::sin(walkTimer) * 22.0f * walkAmount;
		FLegs = s;
		RLegs = -s;

		tail = std::sin(walkTimer * 0.9f) * 10.0f * walkAmount;
		head = std::sin(walkTimer * 1.1f) * 3.0f * walkAmount
			+ bodyLean * 0.15f;
	}
	else {
		auto lerpTo0 = [&](float& v, float k) {
			v += (0.0f - v) * std::min(1.0f, k * dt);
			};
		lerpTo0(FLegs, 6.0f);
		lerpTo0(RLegs, 6.0f);
		lerpTo0(head, 6.0f);
		lerpTo0(tail, 6.0f);
	}
}

// ============ CALLBACK DE MOUSE ============
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xPos;
		lastY = (GLfloat)yPos;
		firstMouse = false;
	}

	GLfloat xOffset = (GLfloat)xPos - lastX;
	GLfloat yOffset = lastY - (GLfloat)yPos;

	lastX = (GLfloat)xPos;
	lastY = (GLfloat)yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}