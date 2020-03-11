#version 450

struct Float2
{
	float x, y;
};

struct Float3
{
	float x, y, z;
};

struct RoadData
{
	Float2 Position;

    float Rotation;

    float Length;

    uint TextureIndex;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 stencilOffset;

layout (location = 2) uniform vec2 stencilScale;

// DATA BUFFERS

layout (std430, binding = 0) buffer ROAD_DATAS
{
	RoadData roadDatas[];	
};

out vec2 coords;

void main()
{
    const vec2 coordinates[6] = {vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f)};

    const vec2 vertices[6] = {vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f)};

    uint vertexIndex = gl_VertexID % 6;
    uint roadIndex = gl_VertexID / 6;

    RoadData roadData = roadDatas[roadIndex];

    float rotation = roadData.Rotation;

    float s = sin(rotation);
	float c = cos(rotation);
	
    vec2 position;
	position.x = vertices[vertexIndex].x * c * roadData.Length - vertices[vertexIndex].y * s * 2.0f;
	position.y = vertices[vertexIndex].x * s * roadData.Length + vertices[vertexIndex].y * c * 2.0f;

	position.x += roadData.Position.x;
    position.y += roadData.Position.y;

    gl_Position = viewMatrix * vec4(position.x - stencilOffset.x, stencilScale.y - (position.y - stencilOffset.y), 0.5f, 1.0f);

    coords = coordinates[vertexIndex];
	coords.x /= 16.0f;
	coords.x *= roadData.Length;
    coords.y += float(roadData.TextureIndex);
	coords.y /= 16.0f;
}