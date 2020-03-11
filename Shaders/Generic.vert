#version 450

struct Float3
{
	float x, y, z;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform uint vertexCount;

layout (location = 2) uniform uint indexCount;

// DATA BUFFERS

layout (std430, binding = 0) buffer VERTEX_POSITIONS
{
	Float3 vertexPositions[];	
};

layout (std430, binding = 1) buffer VERTEX_NORMALS
{
	Float3 vertexNormals[];
};

layout (std430, binding = 2) buffer INDICES
{
	uint indices[];
};

out vec3 position;
out vec3 normal;

void main()
{
	uint index = indices[gl_VertexID];
	
	position.x = vertexPositions[index].x;
	position.y = vertexPositions[index].y;
	position.z = vertexPositions[index].z;
	
	normal.x = vertexNormals[index].x;
	normal.y = vertexNormals[index].y;
	normal.z = vertexNormals[index].z;

	gl_Position = viewMatrix * vec4(position.x, position.y, position.z, 1.0f);
}

