#version 460 core

in vec4 fs_Col;

out vec4 out_Col;

void main()
{
    out_Col = fs_Col;
}