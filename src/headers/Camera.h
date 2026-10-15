#pragma once
#include "ShaderClass.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); 
    
        int width;
        int height;


        bool firstClick = true;


        glm::vec3 getCameraPos();
        glm::vec3 getCameraDirection();

        float speed = 0.3f;
        float sensitivity = 0.5f;

        Camera(int width, int height, glm::vec3 position, GLFWwindow* window); 
        
        void Matrix(float FOVdeg, float nearPlane, float farPlane, ShaderProgram& shader, const char* uniform);
        void Inputs (GLFWwindow* window);
        float getPositionX();
        float getPositionZ();

    private:
        bool mouseCaptured = true;
        bool cursorModeInitialized = false;
        bool escapeWasDown = false;
        bool toggleWasDown = false;
        bool firstMouse = true;

        double lastX = 0.0;
        double lastY = 0.0;

        static void MouseCallback(
            GLFWwindow* window,
            double xpos,
            double ypos
        );
};