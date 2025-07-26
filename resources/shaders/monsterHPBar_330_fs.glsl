#version 330 core

// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec3 ex_Color;
in  vec2 ex_UV;
out vec4 color;

uniform sampler2D myTextureSampler;
uniform float transparency;

void main(void) {
    // Pass through our original color with full opacity.
    //color = vec4(ex_Color,1.0);
    color = texture(myTextureSampler, ex_UV); // + vec4(ex_Color,1.0f);
    if (color.a <= 0.01) {
        discard;
    }
    if (transparency > 0.0) {
        color.a *= transparency;
    }
}
