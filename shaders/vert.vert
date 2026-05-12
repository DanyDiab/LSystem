#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aInstanceMatrix;
layout(std430, binding = 2) buffer RadiiBuffer {
    float radiis[];
};


uniform mat4 projection;
uniform mat4 view;


void main(){

    float t = aPos.y;

    int index = gl_InstanceID;
    float startingRadius = radiis[index];
    float endingRadius = radiis[index + 1];

    float currWidth = mix(startingRadius,endingRadius,t);

    vec3 localPos = vec3(aPos.x * currWidth, aPos.y, aPos.z * currWidth);
    gl_Position = projection * view * aInstanceMatrix * vec4(localPos, 1.0);
}