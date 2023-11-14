#pragma once
#include <glad/glad.h>; 
#include <string> 
#include <fstream> 
#include <sstream> 
#include <iostream>

#include "global.h"

class ShaderProgram
{
public:

    GLuint ID;


    ShaderProgram(const char* vertexPath, const char* fragmentPath);

    void use();

    //设置shader program里不同的uniform变量
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, mat4& mat) const;
    void setVec3(const std::string& name, vec3& vec) const;
    void setVec4(const std::string& name, vec4& vec) const;
};