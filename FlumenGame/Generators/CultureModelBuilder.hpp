#pragma once

class World;

class CultureModelBuilder
{
	static void GenerateRoads(World&);

	static void GenerateTextures(World&);

public:
	static void Generate(World&);
};
