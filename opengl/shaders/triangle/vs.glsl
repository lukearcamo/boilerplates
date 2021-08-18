#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform float time;

out vec3 vColor;

void main() {
    vColor = color;
    vColor.b += sin(time);
    gl_Position = vec4(position, 1.0);
}