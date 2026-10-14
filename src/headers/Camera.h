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


        float speed = 0.5;
        float sensitivity = 100.0f;

        Camera(int width, int height, glm::vec3 position); 
        
        void Matrix(float FOVdeg, float nearPlane, float farPlane, ShaderProgram& shader, const char* uniform);
        void Inputs (GLFWwindow* window);
        float getPositionX();
        float getPositionZ();
};