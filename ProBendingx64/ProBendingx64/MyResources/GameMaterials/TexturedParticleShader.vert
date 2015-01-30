#version 430

//uniform mat4 worldViewProj;

//in vec3 vertex;

//uniform vec4 newcolor;
//out vec4 Color;


//void main() {
//	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
//	Color = newcolor;
	
//}


in layout(location = 0) vec3 vertex;
in vec2 uv0;

uniform mat4 worldViewProj;
uniform vec3 camPos;
out vec2 oUV0;
float particlesPerRadian = 200;
void main()
{
   gl_Position = worldViewProj * vertex;
   oUV0 = uv0;
   
   //gl_PointSize = particlesPerRadian * 20000 / distance( camPos, vertex ); 
  // vec3 ndc = gl_Position.xyz / gl_Position.w;
	
	//float zDist = 1.0/ndc.z;
	
	//gl_PointSize = 20 * zDist;
}