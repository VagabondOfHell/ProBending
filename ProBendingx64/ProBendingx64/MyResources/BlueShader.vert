#version 430

in vec3 vertex;
in vec2 uv0;

uniform mat4 worldViewProj;

out vec2 oUV0;

void main()
{
   gl_Position = worldViewProj * vec4(vertex.xyz, 1.0);
   oUV0 = uv0;
}
