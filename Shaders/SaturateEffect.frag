#version 450

layout (location = 2) uniform float factor;  

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
	vec3 color = texture(diffuse, textureCoordinates).rgb;
	
	float intensity = (color.r + color.g + color.b) / 3.0f;
	
	color = color * (1.0f - factor) + vec3(intensity * factor);

	fragment = vec4(color.rgb, 1.0f);
}
