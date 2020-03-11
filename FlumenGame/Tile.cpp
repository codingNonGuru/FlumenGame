#include "FlumenGame/Tile.hpp"
#include "FlumenGame/Road.hpp"

Road* RoadData::GetFirst() {return FirstRoad_;}

Road* RoadData::GetLast() {return FirstRoad_ + RoadCount_;}

Tile::Tile()
{
	settlement_ = nullptr;

	domain_ = nullptr;
}

Position3 & Tile::GetPosition()
{
	return position_;
}

ReliefTypes Tile::GetRelief()
{
	return landRatio_ > 0.5f ? ReliefTypes::LAND : ReliefTypes::OCEAN;
}

Float Tile::GetLandRatio()
{
	return landRatio_;
}

void Tile::SetLandRatio(Float landRatio)
{
	landRatio_ = landRatio;
}

Float Tile::GetAverageHeight()
{
	return averageHeight_;
}

void Tile::SetAverageHeight(Float averageHeight)
{
	averageHeight_ = averageHeight;
}

Biome & Tile::GetBiome()
{
	return biome_;
}

Settlement* Tile::GetSettlement()
{
	return settlement_;
}

void Tile::SetSettlement(Settlement* settlement)
{
	settlement_ = settlement;
}

Settlement* Tile::GetDomain()
{
	return domain_;
}

void Tile::SetDomain(Settlement* domain)
{
	domain_ = domain;
}

RoadData Tile::GetRoads() const
{
	return roads_;
}

void Tile::SetRoads(RoadData roads)
{
	roads_ = roads;
}
