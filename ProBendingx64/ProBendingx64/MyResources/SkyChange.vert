#version 430

uniform mat4 worldViewProj;

in vec3 vertex;

uniform vec4 newcolor;
out vec4 Color;


void main() {
	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
    Color = newcolor;
}