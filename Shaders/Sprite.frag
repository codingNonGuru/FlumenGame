#version 450

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
	fragment = texture(diffuse, textureCoordinates).rgba;
	
	fragment.a *= opacity;
}
