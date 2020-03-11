#version 450

layout(location = 0) uniform mat4 projMatrix;
layout(location = 1) uniform mat4 depthMatrix;
layout(location = 2) uniform vec3 lightDirection;
layout(location = 3) uniform vec3 cameraPos;
layout(location = 4) uniform vec2 playerPos;
layout(location = 5) uniform mat4 targetDir;
layout(location = 6) uniform float time;
layout(location = 7) uniform float seaLevel;
layout(location = 12) uniform uint indexCount;
layout(location = 13) uniform uint vertexCount;

layout (location = 2) in vec3 faceTangent;
layout (location = 3) in vec3 faceBitangent;
layout (location = 4) in float desertFactor;

out vec3 pos;
out vec4 shadowCoord;
out float desFactor;
out vec3 light;
out vec3 eye;
out vec3 normal;

struct Vector3 {
	float x, y, z;
};

layout (std430, binding = 0) buffer VERTEX_POSITIONS {
	Vector3 vertexPositions[];
};

layout (std430, binding = 1) buffer INDICES {
	unsigned int indices[];
};

layout (std430, binding = 2) buffer NORMALS {
	Vector3 vertexNormals[];
};

layout (std430, binding = 3) buffer TANGENTS {
	Vector3 vertexTangents[];
};  

void main()
{		
	uint vertexIndex = indices[gl_VertexID];
	vec3 vertex;
	vertex.x = vertexPositions[vertexIndex].x;
	vertex.y = vertexPositions[vertexIndex].y;
	vertex.z = vertexPositions[vertexIndex].z;
	
	normal.x = vertexNormals[vertexIndex].x;
	normal.y = vertexNormals[vertexIndex].y;
	normal.z = vertexNormals[vertexIndex].z;
	
	vec3 tangent;
	tangent.x = vertexTangents[vertexIndex].x;
	tangent.y = vertexTangents[vertexIndex].y;
	tangent.z = vertexTangents[vertexIndex].z;
	
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = transpose(mat3(tangent, bitangent, normal));
	light = tbn * (inverse(mat4(1.0f)) * vec4(lightDirection.xyz, 0.0f)).xyz;
	eye = tbn * (cameraPos - vertex);
	
	gl_Position = projMatrix * vec4(vertex.xyz, 1.0f);
	shadowCoord = depthMatrix * vec4(vertex.xyz, 1.0f);
	desFactor = 0.9f;
	pos = vec3(vertex.xyz);
} 