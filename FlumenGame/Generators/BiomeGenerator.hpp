#pragma once

class World;

class BiomeGenerator
{
	static void SetupBuffers(World&);

public:
	static void Generate(World&);
};
