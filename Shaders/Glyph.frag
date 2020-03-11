#version 450

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

in vec4 color;

void main()
{	
	fragment.rgb = color.rgb;

	fragment.a = texture(diffuse, textureCoordinates).r;
	
	//fragment.a *= opacity;
	
	//fragment.a = 0.1f;
}