//Practica 9 
//Guillermo Espinoza Sanchez
//fecha de entrega: 15 de octubre de 2026
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
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ==========================================
// POSICIONES DE LAS 5 FUENTES DE LUZ
// ==========================================
glm::vec3 fogataPos(2.0f, -0.3f, 0.0f);         // 1. Fogata (junto al perro)
glm::vec3 nubesPos(-7.0f, 9.0f, -30.0f);        // 2. Nubes (arriba izquierda)
glm::vec3 casaVentanaPos(1.5f, 0.6f, -13.0f);   // 3. Casa (FRENTE, fuera del edificio)
glm::vec3 luciernagasPos(-2.0f, 0.5f, -16.0f);  // 4. Luciernagas (arbol)
glm::vec3 lunaPos(0.0f, 25.0f, -20.0f);         // 5. Luna (arriba, centrada)
glm::vec3 lunaDir(0.0f, -1.0f, 0.15f);          // Direccion de la luz de la luna

// ==========================================
// VARIABLES DE ANIMACION
// ==========================================
float fireTime = 0.0f;
float fireFlicker = 1.0f;

float luciernagaTime = 0.0f;
float luciernagaFlicker = 1.0f;

float nubeTime = 0.0f;
float nubePulse = 1.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fuentes de Luz - Guillermo Espinoza Sanchez", nullptr, nullptr);
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
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // ==========================================
    // CARGA DE MODELOS (.OBJ)
    // ==========================================
    Model dog((char*)"Models/RedDog.obj");
    Model pasto((char*)"Models/SM_Prop_Grass(1).obj");
    Model casa((char*)"Models/medieval_building.obj");
    Model montana((char*)"Models/mountain_OBJ.obj");
    Model nubes((char*)"Models/cloud.obj");
    Model arbol((char*)"Models/GenTree_105_AE3D_03122023-F2.obj");
    Model luna((char*)"Models/Moon.obj");

    // ==========================================
    // TEXTURAS DE MATERIAL (samplers blancos/grises)
    // ==========================================
    GLuint whiteTexture, grayTexture;
    {
        unsigned char whitePixel[] = { 255, 255, 255, 255 };
        glGenTextures(1, &whiteTexture);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char grayPixel[] = { 128, 128, 128, 255 };
        glGenTextures(1, &grayTexture);
        glBindTexture(GL_TEXTURE_2D, grayTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, grayPixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

    // ==========================================
    // VAO/VBO del cubo para lamparas, fogata, luciernagas
    // ==========================================
    float lampVertices[] = {
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f
    };

    GLuint lampVAO, lampVBO;
    glGenVertexArrays(1, &lampVAO);
    glGenBuffers(1, &lampVBO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        // ANIMACIONES DE LAS LUCES
        fireTime += deltaTime;
        luciernagaTime += deltaTime;
        nubeTime += deltaTime;

        fireFlicker = 0.85f
            + 0.10f * sin(fireTime * 12.0f)
            + 0.05f * sin(fireTime * 27.0f)
            + 0.03f * sin(fireTime * 53.0f);

        luciernagaFlicker = 0.3f + 0.7f * (0.5f + 0.5f * sin(luciernagaTime * 4.0f))
            * (0.5f + 0.5f * sin(luciernagaTime * 7.0f));

        nubePulse = 0.85f + 0.15f * sin(nubeTime * 1.5f);

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();

        // ==========================================
        // CONFIGURACION DEL SHADER DE ILUMINACION
        // ==========================================
        lightingShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"),
            camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, grayTexture);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.3f, -1.0f, -0.2f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.05f, 0.06f, 0.10f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.10f, 0.12f, 0.18f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.05f, 0.05f, 0.08f);

        // LUZ 1: FOGATA 
        glm::vec3 fogataColor(1.0f, 0.5f, 0.15f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"),
            fogataPos.x, fogataPos.y, fogataPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"),
            0.15f, 0.05f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"),
            fogataColor.x * fireFlicker,
            fogataColor.y * fireFlicker,
            fogataColor.z * fireFlicker);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"),
            fogataColor.x * fireFlicker * 0.5f,
            fogataColor.y * fireFlicker * 0.5f,
            fogataColor.z * fireFlicker * 0.5f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.14f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.07f);

        // LUZ 2: NUBES 
        glm::vec3 nubeColor(0.5f, 0.6f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"),
            nubesPos.x, nubesPos.y, nubesPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"),
            0.05f, 0.05f, 0.12f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"),
            nubeColor.x * nubePulse,
            nubeColor.y * nubePulse,
            nubeColor.z * nubePulse);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"),
            nubeColor.x * 0.3f * nubePulse,
            nubeColor.y * 0.3f * nubePulse,
            nubeColor.z * 0.3f * nubePulse);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.022f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0019f);

        // LUZ 3: CASA 
        glm::vec3 casaColor(1.0f, 0.75f, 0.35f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"),
            casaVentanaPos.x, casaVentanaPos.y, casaVentanaPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"),
            0.20f, 0.15f, 0.06f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"),
            casaColor.x * 1.4f, casaColor.y * 1.4f, casaColor.z * 1.4f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"),
            casaColor.x * 0.4f, casaColor.y * 0.4f, casaColor.z * 0.4f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

        // LUZ 4: LUCIERNAGAS 
        glm::vec3 luciernagaColor(1.0f, 1.0f, 0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"),
            luciernagasPos.x, luciernagasPos.y, luciernagasPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"),
            0.05f, 0.05f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"),
            luciernagaColor.x * luciernagaFlicker,
            luciernagaColor.y * luciernagaFlicker,
            luciernagaColor.z * luciernagaFlicker);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"),
            0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.35f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.44f);

        // LUZ 5: LUNA 
        glm::vec3 lunaColor(0.75f, 0.85f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"),
            lunaPos.x, lunaPos.y, lunaPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"),
            lunaDir.x, lunaDir.y, lunaDir.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"),
            0.02f, 0.03f, 0.05f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"),
            lunaColor.x * 0.35f, lunaColor.y * 0.38f, lunaColor.z * 0.45f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"),
            lunaColor.x * 0.10f, lunaColor.y * 0.12f, lunaColor.z * 0.15f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.007f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.0002f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(50.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(65.0f)));

        // ---------- MATERIAL ----------
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        // ==========================================
        // DIBUJO DE LA ESCENA (paisaje)
        // ==========================================
        glm::mat4 model = glm::mat4(1.0f);

        // Pasto
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.4f, -15.0f));
        model = glm::scale(model, glm::vec3(40.0f, 1.0f, 40.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        pasto.Draw(lightingShader);

        // Casa
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.5f, -0.4f, -15.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
        model = glm::rotate(model, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        casa.Draw(lightingShader);

        // Montana
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.4f, -40.0f));
        model = glm::scale(model, glm::vec3(0.004f, 0.009f, 0.004f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        montana.Draw(lightingShader);

        // Nube 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, 10.0f, -30.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        nubes.Draw(lightingShader);

        // Nube 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.0f, 15.0f, -40.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        nubes.Draw(lightingShader);

        // Nube 3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(9.0f, 12.0f, -20.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        nubes.Draw(lightingShader);

        // Arbol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -0.4f, -16.0f));
        model = glm::scale(model, glm::vec3(0.26f, 0.26f, 0.26f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        arbol.Draw(lightingShader);

        // Perro (protagonista, en el origen)
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        dog.Draw(lightingShader);

        // ============================================================
        // LUNA: dibujada con lightingShader (CON TEXTURA ORIGINAL)
        // Boost temporal de dirLight para iluminarla
        // ============================================================
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.0f, -1.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.9f, 0.92f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.9f, 0.92f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.0f, 0.0f, 0.0f);

        glm::mat4 lunaModel = glm::mat4(1.0f);
        lunaModel = glm::translate(lunaModel, lunaPos);
        lunaModel = glm::scale(lunaModel, glm::vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(lunaModel));
        luna.Draw(lightingShader);

        // Restaurar luz direccional original
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.3f, -1.0f, -0.2f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.05f, 0.06f, 0.10f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.10f, 0.12f, 0.18f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.05f, 0.05f, 0.08f);

        // ==========================================
        // DIBUJO DE LAS REPRESENTACIONES VISUALES DE LAS LUCES
        // ==========================================
        lampShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(lampVAO);

        // ---------- FOGATA: troncos + llamas ----------
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 0.6f, 0.25f, 0.08f);

        glm::mat4 tronco1 = glm::mat4(1.0f);
        tronco1 = glm::translate(tronco1, fogataPos + glm::vec3(-0.15f, 0.0f, 0.0f));
        tronco1 = glm::rotate(tronco1, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        tronco1 = glm::scale(tronco1, glm::vec3(0.4f, 0.08f, 0.08f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tronco1));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 tronco2 = glm::mat4(1.0f);
        tronco2 = glm::translate(tronco2, fogataPos + glm::vec3(0.15f, 0.0f, 0.0f));
        tronco2 = glm::rotate(tronco2, glm::radians(-35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        tronco2 = glm::scale(tronco2, glm::vec3(0.4f, 0.08f, 0.08f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tronco2));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 tronco3 = glm::mat4(1.0f);
        tronco3 = glm::translate(tronco3, fogataPos + glm::vec3(0.0f, 0.05f, 0.0f));
        tronco3 = glm::rotate(tronco3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        tronco3 = glm::scale(tronco3, glm::vec3(0.35f, 0.06f, 0.06f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tronco3));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Llama exterior
        float llamaScale = 0.15f + 0.05f * fireFlicker;
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 1.0f, 0.5f, 0.15f);
        glm::mat4 llama = glm::mat4(1.0f);
        llama = glm::translate(llama, fogataPos + glm::vec3(0.0f, 0.25f, 0.0f));
        llama = glm::scale(llama, glm::vec3(llamaScale, llamaScale * 1.5f, llamaScale));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(llama));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Nucleo de la llama
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 1.0f, 0.95f, 0.5f);
        glm::mat4 llama2 = glm::mat4(1.0f);
        llama2 = glm::translate(llama2, fogataPos + glm::vec3(0.0f, 0.30f, 0.0f));
        llama2 = glm::scale(llama2, glm::vec3(llamaScale * 0.6f, llamaScale * 1.0f, llamaScale * 0.6f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(llama2));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ---------- NUBES ----------
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"),
            0.5f * nubePulse, 0.6f * nubePulse, 1.0f * nubePulse);
        glm::mat4 nubeLuz = glm::mat4(1.0f);
        nubeLuz = glm::translate(nubeLuz, nubesPos);
        nubeLuz = glm::scale(nubeLuz, glm::vec3(0.6f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(nubeLuz));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ---------- CASA ----------
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 1.0f, 0.75f, 0.35f);
        glm::mat4 ventanaLuz = glm::mat4(1.0f);
        ventanaLuz = glm::translate(ventanaLuz, casaVentanaPos);
        ventanaLuz = glm::scale(ventanaLuz, glm::vec3(0.25f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(ventanaLuz));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ---------- LUCIERNAGAS  ----------
        glm::vec3 luciernagaOffsets[5] = {
            glm::vec3(-0.3f,  0.4f,  0.2f),
            glm::vec3(0.4f,  0.6f, -0.3f),
            glm::vec3(0.1f,  0.9f,  0.4f),
            glm::vec3(-0.5f,  0.7f, -0.2f),
            glm::vec3(0.2f,  1.1f,  0.0f)
        };

        for (int i = 0; i < 5; i++)
        {
            float phase = luciernagaTime * 3.0f + i * 1.7f;
            float individual = 0.3f + 0.7f * (0.5f + 0.5f * sin(phase));

            glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"),
                1.0f * individual,
                1.0f * individual,
                0.3f * individual);

            glm::mat4 luciernaga = glm::mat4(1.0f);
            luciernaga = glm::translate(luciernaga, luciernagasPos + luciernagaOffsets[i]);
            luciernaga = glm::scale(luciernaga, glm::vec3(0.07f));
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(luciernaga));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
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