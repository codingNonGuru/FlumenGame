#pragma once

#include "FlumenCore/Conventions.hpp"

class World;
class DataBuffer;

class CultureGenerator
{
	static void GenerateSettlements(World&);

	static void GenerateLinks(World&);

	static void GenerateRoads(World&);

	static void GenerateDomains(World&);

public:
	static void Generate(World&);
};
