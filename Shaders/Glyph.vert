#version 450

struct GlyphData
{
	vec4 Color;
	
	vec2 Scale;

	vec2 Position;

	vec2 TextureOffset;

	vec2 TextureScale;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 2) uniform float depth;

// DATA BUFFERS

layout (std430, binding = 0) buffer GLYPHS
{
	GlyphData glyphs[];	
};

// TEXTURES

// OUTPUT

out vec2 textureCoordinates;

out vec4 color;

void main()
{	
	const vec2 vertices[6] = {vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f)};
	
	uint vertexIndex = gl_VertexID % 6;
	
	uint glyphIndex = gl_VertexID / 6;
	
	vec2 glyphPosition = glyphs[glyphIndex].Position;
	
	vec2 glyphScale = glyphs[glyphIndex].Scale;

	vec2 position = vertices[vertexIndex] * glyphScale + glyphPosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
	
	vec2 textureOffset = glyphs[glyphIndex].TextureOffset;
	
	vec2 textureScale = glyphs[glyphIndex].TextureScale;
	
	textureCoordinates = vertices[vertexIndex] + vec2(0.5f, 0.5f);
	
	textureCoordinates = textureCoordinates * textureScale + textureOffset;
	
	color = glyphs[glyphIndex].Color;
}