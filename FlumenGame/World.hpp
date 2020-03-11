#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenGame/Types.hpp"

class File;
struct WorldParameterSet;
class Tile;
class Settlement;
class Economy;
class Camera;
struct Link;
struct Road;

struct WorldObject
{
	void* Object_;

	WorldObjects Type_;

	WorldObject() {}

	WorldObject(void* Object, WorldObjects Type) : Object_(Object), Type_(Type) {}

	static WorldObject DEFAULT;
};

class World
{
	bool isUpdating_;

	Size size_;

	Grid <Tile> tiles_;

	Array <Settlement> settlements_;

	Array <Economy> economies_;

	Array <Link> links_;

	Array <Road> roads_;

	Grid <Float> heightMap_;

	int detailResolution_;

	float resolutionFactor_;

	Array <WorldObject> collisionAttempts_;

	void UpdateSettlements();

public:
	World();

	World(File*);

	World(const WorldParameterSet&);

	Grid <Tile> & GetTiles();

	Size GetSize();

	void SetSize(Size);

	Array <Settlement> & GetSettlements();

	Array <Economy> & GetEconomies();

	Array <Link> & GetLinks();

	Array <Road> & GetRoads();

	Float GetHeight(Float, Float);

	Grid <Float> & GetHeightMap();

	void SetDetailResolution(int);

	WorldObject ProcessSelection(Camera*, Float3, Float2);

	void Update();

	bool IsUpdating() {return isUpdating_;}

	void Start() {isUpdating_ = true;}

	void Stop() {isUpdating_ = false;}
};
