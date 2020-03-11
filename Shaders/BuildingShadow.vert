#version 450

struct Float3
{
	float x, y, z;
};

struct BuildingData
{
	Float3 Position_;

	float Rotation_;

	uint MeshIndex_;

	uint SettlementIndex_;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 depthMatrix;

layout (location = 2) uniform uint indexCount;

// DATA BUFFERS

layout (std430, binding = 0) buffer VERTEX_POSITIONS
{
	Float3 vertexPositions[];	
};

layout (std430, binding = 2) buffer INDICES
{
	uint indices[];
};

layout (std430, binding = 3) buffer BUILDING_INDICES
{
	uint buildingIndices[];
};

layout (std430, binding = 4) buffer BUILDING_DATAS
{
	BuildingData buildingDatas[];
};

void main()
{
	uint buildingIndex = buildingIndices[gl_VertexID / indexCount];
	Float3 buildingPosition = buildingDatas[buildingIndex].Position_;

	uint index = indices[buildingDatas[buildingIndex].MeshIndex_ * indexCount + gl_VertexID % indexCount];
	
	float rotation = buildingDatas[buildingIndex].Rotation_; 
	
	float s = sin(rotation);
	float c = cos(rotation);
	
	vec3 position;
	
	position.x = vertexPositions[index].x * c - vertexPositions[index].y * s;
	position.x += buildingPosition.x;
	
	position.y = vertexPositions[index].x * s + vertexPositions[index].y * c;
	position.y += buildingPosition.y;
	
	position.z = vertexPositions[index].z;
	position.z += buildingPosition.z;

	gl_Position = depthMatrix * vec4(position.x, position.y, position.z, 1.0f);
}