#version 450

layout (location = 1) uniform vec2 screenSize;  

layout (location = 2) uniform int mode;  

layout (location = 3) uniform int kernelSide;

// BUFFERS

layout (std430, binding = 0) buffer SCREEN_PIXELS {
	float weights[];
};

// TEXTURES

uniform sampler2D source; 

layout (location = 0) out vec4 fragment;

void main()
{	
	ivec2 coords = ivec2(gl_FragCoord.x, gl_FragCoord.y);
	
	vec4 color = vec4(0.0f);
	vec4 sum = vec4(0.0f);

	int kernelSpread = 2 * kernelSide + 1;

	if(mode == 0) 
	{
		for(int x = 0; x < kernelSpread; ++x) 
		{
			ivec2 newCoords = coords + ivec2(x - kernelSide, 0);
			color = texture(source, vec2(float(newCoords.x) / screenSize.x, float(newCoords.y) / screenSize.y)).rgba;

			sum += color * weights[x];
		}	
	}
	else if(mode == 1)
	{
		for(int y = 0; y < kernelSpread; ++y) 
		{
			ivec2 newCoords = coords + ivec2(0, y - kernelSide);
			color = texture(source, vec2(float(newCoords.x) / screenSize.x, float(newCoords.y) / screenSize.y)).rgba;	

			sum += color * weights[y];
		}
	}
	
	fragment = vec4(sum.rgb, 1.0f);
}