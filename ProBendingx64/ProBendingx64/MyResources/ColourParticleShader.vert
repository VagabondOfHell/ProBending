#version 430

uniform mat4 worldViewProj;

in vec3 vertex;
in vec4 colour;

uniform vec3 camPos;

out vec4 Color;
in vec4 blendIndices;//stores scale

float pixelsPerRadian = 200;

void main() {
	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
	Color = colour;
	
	
		
	//gl_PointSize = pixels_per_radian * point_diameter / distance( camera, pointcenter ); 
	gl_PointSize = pixelsPerRadian * blendIndices.x / distance( camPos, vertex ); 
}