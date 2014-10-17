
#version 430

uniform sampler2D diffuseMap;

in vec2 oUV0;

void main ()
{
	vec4 _color = texture2D(diffuseMap, oUV0);

   gl_FragColor = vec4(0.0, _color.gba);
}
