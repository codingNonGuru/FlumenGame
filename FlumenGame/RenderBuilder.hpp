#pragma once

class RenderBuilder
{
	static RenderBuilder* instance_;

	RenderBuilder();

public:
	static void Initialize();

	static const Size SHADOW_MAP_SIZE;

	static const float SHADOW_MAP_SIZE_MODIFIER;

	static const Size ROAD_STENCIL_SIZE;
};
