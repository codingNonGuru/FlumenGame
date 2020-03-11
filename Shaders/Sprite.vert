#version 450

struct Float3
{
	float x, y, z;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 spritePosition;

layout (location = 2) uniform vec2 spriteSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

// TEXTURES

// OUTPUT

out vec2 textureCoordinates;

void main()
{	
	const vec2 vertices[6] = {vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f)};
	
	textureCoordinates = vertices[gl_VertexID] + vec2(0.5f, 0.5f);

	vec2 position = vertices[gl_VertexID] * spriteSize + spritePosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}