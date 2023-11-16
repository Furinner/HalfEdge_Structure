#version 460 core


in vec3 fs_Nor;
in vec3 fs_Col;

uniform vec3 camFront;

out vec4 out_Col;

void main()
{
    vec3 lightDir = normalize(vec3(1.f, 1.f, 1.f));
    float multiplier = (dot(lightDir, fs_Nor) + 1.f) * 0.5f;
    out_Col = vec4(multiplier * fs_Col, 1.f);
}