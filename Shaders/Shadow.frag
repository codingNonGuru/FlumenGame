#version 450

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
	float alpha = texture(diffuse, textureCoordinates).r;
	
	fragment = vec4(0.0f, 0.0f, 0.0f, alpha);
}