#version 430

uniform sampler2D tex;

in vec4 Color;

//void main()
//{
//	gl_FragColor = texture2D(tex, oUV);// vec4(0.0f, 0.0f, 1.0f, 1.0f);
//}

in vec2 oUV0;

void main ()
{
  gl_FragColor = Color;// texture2D(tex, oUV0);
}