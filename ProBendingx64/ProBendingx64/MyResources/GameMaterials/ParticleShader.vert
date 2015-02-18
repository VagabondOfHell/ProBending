#version 430

uniform mat4 worldViewProj;
uniform vec3 camPos;

in vec4 vertex;

#if USE_TEXTURE>=1
//These will be used for animated textures
	in vec4 uv0;
	flat out int imageIndex;
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
	
	#if USE_TEXTURE>=1
	 imageIndex = int(uv0.x);
	 #endif
	 
	 float particleSize = 1.0;
	 
	 #if PER_PARTICLE_SCALE==1
		particleSize = vertex.w;
	#endif
	
	gl_PointSize = (pixelsPerRadian * particleSize) / distance( camPos, vertex.xyz); 
}
