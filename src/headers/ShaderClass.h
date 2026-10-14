#pragma once
#include <glad/gl.h>

#include<glad/gl.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

class ShaderProgram{
    public:
        GLuint ID;

        ShaderProgram(const char* vertexPath, const char* fragmentPath);
        void Activate();
        void Delete();
};