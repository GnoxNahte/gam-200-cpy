#version 300 es
precision mediump float;

uniform vec3 MaterialColor;

out vec4 FragColor;

void main() {
	FragColor = vec4( MaterialColor, 1.0 );
}