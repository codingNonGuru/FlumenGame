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

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform mat4 depthMatrix;

layout (location = 2) uniform uint indexCount;

layout (location = 4) uniform uint connectionCount;

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

layout (std430, binding = 3) buffer BUILDING_INDICES
{
	uint buildingIndices[];
};

layout (std430, binding = 4) buffer BUILDING_DATAS
{
	BuildingData buildingDatas[];
};

layout (std430, binding = 5) buffer TEXTURE_INDICES
{
	float textureIndices[];
};

layout (std430, binding = 6) buffer CONNECTION_INDICES
{
	uint connectionIndices[];
};

out vec3 position;
out vec3 normal;
out vec4 shadowCoord;
out float textureIndex;
out float isConnection;

void main()
{
	uint buildingIndex = buildingIndices[gl_VertexID / indexCount];
	Float3 buildingPosition = buildingDatas[buildingIndex].Position_;

	uint index = indices[buildingDatas[buildingIndex].MeshIndex_ * indexCount + gl_VertexID % indexCount];
	
	float rotation = buildingDatas[buildingIndex].Rotation_; 
	
	float s = sin(rotation);
	float c = cos(rotation);
	
	position.x = vertexPositions[index].x * c - vertexPositions[index].y * s;
	position.x += buildingPosition.x;
	
	position.y = vertexPositions[index].x * s + vertexPositions[index].y * c;
	position.y += buildingPosition.y;
	
	position.z = vertexPositions[index].z;
	position.z += buildingPosition.z;
	position.z += 0.03f;
	
	normal.x = vertexNormals[index].x * c - vertexNormals[index].y * s;
	normal.y = vertexNormals[index].x * s + vertexNormals[index].y * c;
	normal.z = vertexNormals[index].z;
	
	textureIndex = textureIndices[index];

	gl_Position = viewMatrix * vec4(position.x, position.y, position.z, 1.0f);
	shadowCoord = depthMatrix * vec4(position.xyz, 1.0f);

	isConnection = 0.0f;
	for(int connectionIndex = 0; connectionIndex < connectionCount; ++connectionIndex)
	{
		if(connectionIndices[connectionIndex] == buildingDatas[buildingIndex].SettlementIndex_)
			isConnection = 1.0f;
	}
}