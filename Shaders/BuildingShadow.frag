#version 450

layout (location = 0) out float fragment;

void main()
{
	fragment = gl_FragCoord.z;
}