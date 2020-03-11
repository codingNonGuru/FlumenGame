#version 450

struct Float3
{
	float x, y, z;
};

struct BuildingData
{
	Float3 Position;

	float Rotation;

	uint MeshIndex;

	uint SettlementIndex;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 stencilOffset;

layout (location = 2) uniform vec2 stencilScale;

// DATA BUFFERS

layout (std430, binding = 0) buffer BUILDING_INDICES
{
	uint buildingIndices[];
};

layout (std430, binding = 1) buffer BUILDING_DATAS
{
	BuildingData buildingDatas[];
};

out vec2 pos;

void main()
{
    const vec2 vertices[6] = {vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f)};

    uint vertexIndex = gl_VertexID % 6;
    uint buildingIndex = buildingIndices[gl_VertexID / 6];

    BuildingData data = buildingDatas[buildingIndex];
	
    vec2 position;
	position.x = vertices[vertexIndex].x * 1.5f + data.Position.x;
	position.y = vertices[vertexIndex].y * 1.5f + data.Position.y;

    gl_Position = viewMatrix * vec4(position.x - stencilOffset.x, stencilScale.y - (position.y - stencilOffset.y), 0.5f, 1.0f);

    pos.x = vertices[vertexIndex].x;
    pos.y = vertices[vertexIndex].y;
}