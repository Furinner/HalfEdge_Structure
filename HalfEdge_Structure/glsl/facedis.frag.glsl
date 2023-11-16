#version 460 core


in vec3 fs_Nor;
in vec3 fs_Col;

uniform vec3 camFront;

out vec4 out_Col;

void main()
{
    if(dot(camFront, fs_Nor) <= 0.f){
        out_Col = vec4(fs_Col, 1.f);
    }else{
        out_Col = vec4(fs_Col * 0.2f, 1.f);
    }
}