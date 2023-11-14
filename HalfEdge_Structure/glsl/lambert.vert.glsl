#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 fs_Col; // 为片段着色器指定一个颜色输出

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);; // 注意我们如何把一个vec3作为vec4的构造器的参数
    fs_Col = vec4(0.0, 0.0, 0.0, 1.0); // 把输出变量设置为暗红色
}