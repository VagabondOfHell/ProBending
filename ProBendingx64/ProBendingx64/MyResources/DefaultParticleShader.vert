#version 430

uniform mat4 worldViewProj;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 world;

in vec3 vertex;
uniform vec3 camPos;
uniform vec4 newcolor;
out vec4 Color;

float pixelsPerRadian = 200;

void main() {
//worldViewProj * vec4(vertex.xyz, 1.0);
	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);//  worldViewProj * vec4(vertex.xyz, 1.0);
	Color = newcolor;
	
	//gl_PointSize = pixels_per_radian * point_diameter / distance( camera, pointcenter ); 
	gl_PointSize = pixelsPerRadian * 50 / distance( camPos, vertex ); 
}