#version 300 es
layout(location = 0) in vec3 VertexPosition;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

out vec2    vertexPos;
out vec3    normalCoord;
out vec3    eyeCoord;

void main()
{
    normalCoord = normalize ( NormalMatrix * vec3(0.0, 1.0, 0.0) );
    eyeCoord    = vec3 ( ModelViewMatrix * vec4(VertexPosition, 1.0) );
    vertexPos = VertexPosition.xz;
    gl_Position   = ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
}
