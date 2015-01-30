#version 430

uniform mat4 worldViewProj;
uniform mat4 inverseWVP;

in vec3 vertex;
uniform vec3 camPos;
uniform vec4 newcolor;
out vec4 Color;
in vec4 blendIndices;//stores scale

float pixelsPerRadian = 100;

void main() {
	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
	Color = vec4(0.0f, 1.0f, 0.0f, 0.40f);// newcolor;
	float size = blendIndices.x;
	
	if(size > 30)
	    size = 30;
	//gl_PointSize = pixels_per_radian * point_diameter / distance( camera, pointcenter ); 
	gl_PointSize = pixelsPerRadian * (size / distance( vertex, camPos)); 
}