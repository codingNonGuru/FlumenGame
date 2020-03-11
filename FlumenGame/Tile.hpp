#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenGame/Types.hpp"

class Settlement;
struct Road;

struct Biome
{
	float Productivity_;

	float ForestCover_;
};

struct RoadData
{
	Road* FirstRoad_;

	Length RoadCount_;

public:
	RoadData() {}

	RoadData(Road* FirstRoad, Length RoadCount) : FirstRoad_(FirstRoad), RoadCount_(RoadCount) {}

	Road* GetFirst();

	Road* GetLast();
};

class Tile
{
	Position3 position_;

	Float averageHeight_;

	Float landRatio_;

	Biome biome_;

	Settlement* settlement_;

	Settlement* domain_;

	RoadData roads_;

public:
	Tile();

	Position3 & GetPosition();

	ReliefTypes GetRelief();

	Float GetLandRatio();

	void SetLandRatio(Float);

	Float GetAverageHeight();

	void SetAverageHeight(Float);

	Biome & GetBiome();

	Settlement* GetSettlement();

	void SetSettlement(Settlement*);

	Settlement* GetDomain();

	void SetDomain(Settlement*);

	RoadData GetRoads() const;

	void SetRoads(RoadData);
};
