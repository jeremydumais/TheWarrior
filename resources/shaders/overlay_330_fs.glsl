#version 330 core

precision highp float;

uniform vec3 overlayColor;
uniform float overlayOpacity;

out vec4 color;

void main() {
    color = vec4(overlayColor, overlayOpacity);
}