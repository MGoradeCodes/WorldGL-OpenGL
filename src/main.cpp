#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "headers/VAO.h"
#include "headers/VBO.h"
#include "headers/EBO.h"
#include "headers/Mesh.h"
#include "headers/ShaderClass.h"
#include <glm/glm.hpp>
#include <vector>
#include "headers/Camera.h"
#include "headers/Chunk.h"
#include "headers/Texture.h"
#include "headers/World.h"
#include "headers/stb.h"

void setWindowIcon(GLFWwindow* window, const char* filePath);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

using std::vector;

int main()
{
    // GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
        800, 600, "Minecraft In OpenGL!", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD 2
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    ShaderProgram shaderProgram("shaders/default.vert", "shaders/default.frag");

    vector<float> vertices = {
        // Front face (+Z)
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        // Back face (-Z)
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        // Left face (-X)
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        // Right face (+X)
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        // Bottom face (-Y)
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,

        // Top face (+Y)
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f
    };
    vector<unsigned int> indices = {
        // Front
        0,  1,  2,  0,  2,  3,
        // Back
        4,  5,  6,  4,  6,  7,
        // Left
        8,  9, 10,  8, 10, 11,
        // Right
        12, 13, 14, 12, 14, 15,
        // Bottom
        16, 17, 18, 16, 18, 19,
        // Top
        20, 21, 22, 20, 22, 23
    };
    World world;
	Texture texture("textures/terrain.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	texture.texUnit(shaderProgram, "tex0", 0);


    glEnable(GL_DEPTH_TEST);

    Camera camera(800, 600, glm::vec3(0, 0, 2));
    glfwSwapInterval(1);

    double prevTime = 0.0;
    double currTime = 0.0;
    double diff = 0.0;
    unsigned int counter = 0;

    glfwSwapInterval(1);
    setWindowIcon(window, "textures/grass.png");

    while (!glfwWindowShouldClose(window))
    {
        camera.Inputs(window);

        float cameraX = camera.getPositionX();
        float cameraZ = camera.getPositionZ();
        currTime = glfwGetTime();
        diff = currTime - prevTime;

        world.Update(cameraX, cameraZ);

        ++counter;

        if (diff >= 1.0 / 30.0) {
            const float fps = (1.0f / diff) * static_cast<float>(counter);
            std::string fpsText = std::to_string(fps);
            std::string newTitle = "Minecraft In Opengl - FPS : " + fpsText;
            glfwSetWindowTitle(window, newTitle.c_str());
            prevTime = currTime;
            counter = 0;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        texture.Bind();

        camera.Matrix(
            45.0f, 0.1f, 100.0f,
            shaderProgram, "camMatrix"
        );

        world.Draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    texture.Delete();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void setWindowIcon(GLFWwindow* window, const char* filePath) {
    GLFWimage images[1];
    int channels;
    
    // Force 4 channels to load the image data as RGBA (required by GLFW)
    stbi_set_flip_vertically_on_load(false); 
    images[0].pixels = stbi_load(filePath, &images[0].width, &images[0].height, &channels, 4); 
    
    if (images[0].pixels != nullptr) {
        // Apply the loaded image array to the specified window
        glfwSetWindowIcon(window, 1, images);
        
        // Free the allocated pixel buffer once it's set
        stbi_image_free(images[0].pixels);
    } 
}