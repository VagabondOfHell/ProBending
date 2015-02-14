#version 430

uniform mat4 worldViewProj;
uniform vec3 camPos;

in vec4 vertex;

#if USE_TEXTURE==1
//These will be used for animated textures
	in vec2 uv0;
	out vec2 oUV0;
#endif

#if PER_PARTICLE_COLOUR==1
	in vec4 colour;
#else
	uniform vec4 colour;
#endif

out vec4 Color;

float pixelsPerRadian = 200;

void main() {
	gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
	
	Color = colour;
	
	#if USE_TEXTURE==1
	 oUV0 = uv0;
	 #endif
	 
	 float particleSize = 1.0;
	 
	 #if PER_PARTICLE_SCALE==1
		particleSize = vertex.w;
	#endif
	
	gl_PointSize = (pixelsPerRadian * particleSize) / distance( camPos, vertex.xyz); 
}
