#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aInstanceMatrix;

uniform mat4 projection;
uniform mat4 view;

out vec3 vNormal;

void main() {
    vec3 localNormal = normalize(vec3(aPos.x, aPos.y, 0.0));
    vNormal = mat3(aInstanceMatrix) * localNormal;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}