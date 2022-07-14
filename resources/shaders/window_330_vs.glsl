#version 330 core

// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in  vec2 in_Position;
in  vec3 in_Color;
in  vec2 in_VertexUV;
out vec3 ex_Color;
out vec2 ex_UV;

uniform vec2 translation;

void main(void) {
    // Since we are using flat lines, our input only had two points: x and y.
    // Set the Z coordinate to 0 and W coordinate to 1

    gl_Position = vec4(in_Position.x + translation.x, in_Position.y + translation.y, 0.0, 1.0);

    // We're simply passing the color through unmodified
    ex_Color = in_Color;

    // UV of the vertex.
    ex_UV = in_VertexUV;
}