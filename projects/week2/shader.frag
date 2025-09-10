#version 330 core

in vec3 fColor;
out vec4 diffuseColor;

void main() {
    diffuseColor = vec4(fColor, 1.0);
}