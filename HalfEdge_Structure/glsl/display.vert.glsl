#version 460 core

layout (location = 0) in vec3 vs_Pos;
layout (location = 1) in vec3 vs_Col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fs_Col; // 为片段着色器指定一个颜色输出

void main()
{
    gl_Position = proj * view * model * vec4(vs_Pos, 1.0); // 注意我们如何把一个vec3作为vec4的构造器的参数
    fs_Col = vs_Col;
}