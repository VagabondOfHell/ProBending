#version 430

in vec4 Color;
out vec4 colour;

#if USE_TEXTURE>=1
flat in int imageIndex;

uniform sampler2D tex0;

	#if USE_TEXTURE>=2
	uniform sampler2D tex1;
		#if USE_TEXTURE>=3
		uniform sampler2D tex2;
			#if USE_TEXTURE>=3
			uniform sampler2D tex3;
			#if USE_TEXTURE>=4
			uniform sampler2D tex4;

			#endif
			#endif
		#endif
	#endif
#endif

void main ()
{
	#if USE_TEXTURE>=1 
	if(imageIndex == 0)
		colour = texture2D(tex0, gl_PointCoord);
	#if USE_TEXTURE>=2
	else if(imageIndex == 1)
		colour = texture2D(tex1, gl_PointCoord);
	#if USE_TEXTURE>=3
	else if(imageIndex == 2)
		colour = texture2D(tex2, gl_PointCoord);
	#if USE_TEXTURE>=4
	else if(imageIndex == 3)
		colour = texture2D(tex3, gl_PointCoord);
	#if USE_TEXTURE>=5
	else if(imageIndex == 4)
		colour = texture2D(tex4, gl_PointCoord);	
		#if PER_PARTICLE_COLOUR==1
			colour *= Color;			
		#endif
	#else 
		colour = Color;
	#endif
	#endif
	#endif
	#endif
	#endif
}