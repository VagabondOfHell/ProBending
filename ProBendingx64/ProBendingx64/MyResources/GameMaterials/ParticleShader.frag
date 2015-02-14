#version 430

uniform sampler2D tex;

in vec4 Color;
out vec4 colour;

#if USE_TEXTURE==1
in vec2 oUV0;
#endif

void main ()
{
	#if USE_TEXTURE==1 
		#if PER_PARTICLE_COLOUR==1
			colour = texture2D(tex, gl_PointCoord) * Color;
		#else
			colour = texture2D(tex, gl_PointCoord);
		#endif
	#else 
		colour = Color;
	#endif
}