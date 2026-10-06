//Practica 8
//Guillermo Espinoza Sanchez
//Fecha de entrega: 06 de octubre de 2026
//319205459

// Std. Includes
#include <string>
#include <iostream>
#include <cmath> 

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ==========================================
// VARIABLES PARA LA ORBITA 
// ==========================================

float orbitAngle = 0.0f;
float orbitRadius = 25.0f;
float orbitSpeed = 1.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Materiales e Iluminacion - Guillermo Espinoza Sanchez", nullptr, nullptr);
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
    glEnable(GL_DEPTH_TEST);

    // ==========================================
    // SHADERS
    // ==========================================
    // shader: Dibuja astros respetando texturas sin verse afectados por sombras
    Shader shader("Shader/modelLoading.vs", "Shader/modelLoading.frag");
    // lightingShader: Dibuja escenario y perro, reaccionando a las luces
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");

    // ==========================================
    // CARGA DE MODELOS (.OBJ)
    // ==========================================
    // Modelo Principal
    Model dog((char*)"Models/RedDog.obj");

    // Entorno
    Model pasto((char*)"Models/SM_Prop_Grass(1).obj");
    Model casa((char*)"Models/medieval_building.obj");
    Model montana((char*)"Models/mountain_OBJ.obj");
    Model nubes((char*)"Models/cloud.obj");
    Model arbol((char*)"Models/GenTree_105_AE3D_03122023-F2.obj");

    // Astros
    Model luna((char*)"Models/Moon.obj");
    Model sol((char*)"Models/Sun.obj");

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        // LOGICA DE LA ORBITA 
        glm::vec3 pivot(0.0f, 0.0f, -15.0f);
        glm::vec3 sunPos = pivot + glm::vec3(orbitRadius * cos(orbitAngle), orbitRadius * sin(orbitAngle), 0.0f);
        glm::vec3 moonPos = pivot + glm::vec3(orbitRadius * cos(orbitAngle + 3.14159f), orbitRadius * sin(orbitAngle + 3.14159f), 0.0f);

        // CONFIGURACIÓN DEL CIELO
        if (sunPos.y > pivot.y) {
            glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        }
        else {
            glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();

        // CONFIGURACION DE LAS LUCES (SOL Y LUNA)
        lightingShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // LUZ 1: SOL 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.position"), sunPos.x, sunPos.y, sunPos.z);
        if (sunPos.y > pivot.y) {
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 0.3f, 0.3f, 0.3f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 1.0f, 0.9f, 0.7f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 1.0f, 0.9f, 0.7f);
        }
        else {
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 0.0f, 0.0f, 0.0f);
        }

        // LUZ 2: LUNA 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.position"), moonPos.x, moonPos.y, moonPos.z);
        if (moonPos.y > pivot.y) {
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.ambient"), 0.01f, 0.01f, 0.02f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.diffuse"), 0.4f, 0.5f, 0.8f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.specular"), 0.4f, 0.5f, 0.8f);
        }
        else {
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.ambient"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.diffuse"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.specular"), 0.0f, 0.0f, 0.0f);
        }

        // MATERIALES DEL ENTORNO
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 0.2f, 0.2f, 0.2f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

        glm::mat4 modelPasto = glm::mat4(1.0f);
        modelPasto = glm::translate(modelPasto, glm::vec3(0.0f, -0.4f, -15.0f));
        modelPasto = glm::scale(modelPasto, glm::vec3(40.0f, 1.0f, 40.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelPasto));
        pasto.Draw(lightingShader);

        glm::mat4 modelCasa = glm::mat4(1.0f);
        modelCasa = glm::translate(modelCasa, glm::vec3(1.5f, -0.4f, -15.0f));
        modelCasa = glm::scale(modelCasa, glm::vec3(0.25f, 0.25f, 0.25f));
        modelCasa = glm::rotate(modelCasa, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelCasa));
        casa.Draw(lightingShader);

        glm::mat4 modelMontana = glm::mat4(1.0f);
        modelMontana = glm::translate(modelMontana, glm::vec3(0.0f, -0.4f, -40.0f));
        modelMontana = glm::scale(modelMontana, glm::vec3(0.004f, 0.009f, 0.004f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMontana));
        montana.Draw(lightingShader);

        glm::mat4 modelNube1 = glm::mat4(1.0f);
        modelNube1 = glm::translate(modelNube1, glm::vec3(-7.0f, 10.0f, -30.0f));
        modelNube1 = glm::scale(modelNube1, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelNube1));
        nubes.Draw(lightingShader);

        glm::mat4 modelNube2 = glm::mat4(1.0f);
        modelNube2 = glm::translate(modelNube2, glm::vec3(7.0f, 15.0f, -40.0f));
        modelNube2 = glm::scale(modelNube2, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelNube2));
        nubes.Draw(lightingShader);

        glm::mat4 modelNube3 = glm::mat4(1.0f);
        modelNube3 = glm::translate(modelNube3, glm::vec3(9.0f, 12.0f, -20.0f));
        modelNube3 = glm::scale(modelNube3, glm::vec3(0.4f, 0.4f, 0.4f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelNube3));
        nubes.Draw(lightingShader);

        glm::mat4 modelArbol = glm::mat4(1.0f);
        modelArbol = glm::translate(modelArbol, glm::vec3(-2.0f, -0.4f, -16.0f));
        modelArbol = glm::scale(modelArbol, glm::vec3(0.26f, 0.26f, 0.26f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelArbol));
        arbol.Draw(lightingShader);

        // MATERIALES DEL MODELO CENTRAL (PERRO)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

        glm::mat4 model(1);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        dog.Draw(lightingShader);

        //DIBUJO DEL SOL Y LA LUNA
        shader.Use();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // EL SOL
        glm::mat4 modelSol(1.0f);
        modelSol = glm::translate(modelSol, sunPos);
        modelSol = glm::scale(modelSol, glm::vec3(0.3f, 0.3f, 0.3f ));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSol));
        sol.Draw(shader);

        // LA LUNA
        glm::mat4 modelLuna(1.0f);
        modelLuna = glm::translate(modelLuna, moonPos);
        modelLuna = glm::scale(modelLuna, glm::vec3(2.5f, 2.5f, 2.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelLuna));
        luna.Draw(shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions and objects based on user input
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

    if (keys[GLFW_KEY_O])
    {
        orbitAngle += orbitSpeed * deltaTime;
    }
    if (keys[GLFW_KEY_L])
    {
        orbitAngle -= orbitSpeed * deltaTime;
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
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