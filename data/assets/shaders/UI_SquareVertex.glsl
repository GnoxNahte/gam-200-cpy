#version 300 es
layout(location = 0) in vec2 VertexPosition;

uniform mat4 ModelViewProjectionMatrix;

void main()
{
    gl_Position   = ModelViewProjectionMatrix * vec4(VertexPosition, 1.0, 1.0);
}
