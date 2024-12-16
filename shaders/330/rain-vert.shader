#version 330 core

in vec3 myPosition;
in vec3 myNormal;

uniform mat4 rainView;
uniform mat4 rainProjection;

out vec3 rainFragPosition;
out vec3 rainFragNormal;

void main() {
    gl_Position = rainProjection * rainView * vec4(myPosition, 1.0);

    rainFragPosition = myPosition; 
    rainFragNormal = myNormal; 
}