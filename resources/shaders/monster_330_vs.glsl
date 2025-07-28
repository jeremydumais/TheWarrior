#version 330 core

// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in  vec2 in_Position;
in  vec3 in_Color;
in  vec2 in_VertexUV;

out vec3 ex_Color;
out vec2 ex_UV;

uniform vec2 translation;
uniform float widthScale; // New: scale factor for width (1.0 = normal, 0.0 = collapsed)
uniform float screenWidth;

void main(void) {
    float translationTest = ((124.0 / screenWidth) / 10) * ((1.0 - widthScale) * 10);
    // Apply horizontal scaling to X coordinate
    float scaledX = in_Position.x * widthScale;

    // Since we are using flat lines, our input only had two points: x and y.
    // Set the Z coordinate to 0 and W coordinate to 1
    gl_Position = vec4(scaledX - translation.x - translationTest, in_Position.y + translation.y, 0.0, 1.0);

    // We're simply passing the color through unmodified
    ex_Color = in_Color;

    // UV of the vertex.
    ex_UV = vec2(in_VertexUV.x, in_VertexUV.y);
}

