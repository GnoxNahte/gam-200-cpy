#version 300 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 Normal;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

out vec3    normalCoord;
out vec3    eyeCoord;

void main()
{
    normalCoord = normalize ( NormalMatrix * Normal );
    eyeCoord    = vec3 ( ModelViewMatrix * vec4(VertexPosition, 1.0) );
    gl_Position   = ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
}
