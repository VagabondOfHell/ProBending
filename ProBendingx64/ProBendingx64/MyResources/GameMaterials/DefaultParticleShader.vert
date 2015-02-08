#version 430

uniform mat4 worldViewProj;

in vec3 vertex;
uniform vec3 emitterPosition;
uniform float size;
uniform vec3 camPos;
uniform vec4 newcolor;
out vec4 Color;
float pixelsPerRadian = 200;

void main() {
	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
	Color = vec4(0.0f, 1.0f, 0.0f, 0.40f);// newcolor;
	
	//gl_PointSize = pixels_per_radian * point_diameter / distance( camera, pointcenter ); 
	gl_PointSize = pixelsPerRadian * size / length( camPos - vertex ); 
	//gl_PointSize =  blendIndices.x;
}