#version 450

struct Float3
{
	float x, y, z;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 screenSize;  

// DATA BUFFERS

// TEXTURES

// OUTPUT

void main()
{	
	const vec2 vertices[6] = {vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f)};
	
	vec2 position = vertices[gl_VertexID] * screenSize;

	gl_Position = viewMatrix * vec4(position.x, position.y, 0.5f, 1.0f);
}