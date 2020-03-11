#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenGame/World.hpp"
#include "FlumenGame/Tile.hpp"
#include "FlumenGame/Generators/WorldGenerator.hpp"
#include "FlumenGame/Generators/WorldParameterSet.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/Road.hpp"
#include "FlumenGame/Economy.hpp"

WorldObject WorldObject::DEFAULT = WorldObject(nullptr, WorldObjects::NONE);

World::World() {}

World::World(File* file)
{
}

World::World(const WorldParameterSet& parameterSet)
{
	WorldGenerator::Generate(*this, parameterSet);

	collisionAttempts_.Initialize(256);

	isUpdating_ = false;
}

Grid <Tile> & World::GetTiles()
{
	return tiles_;
}

Size World::GetSize()
{
	return size_;
}

void World::SetSize(Size size)
{
	size_ = size;
}

Array <Settlement> & World::GetSettlements()
{
	return settlements_;
}

Array <Economy> & World::GetEconomies()
{
	return economies_;
}

Array <Link> & World::GetLinks()
{
	return links_;
}

Array <Road> & World::GetRoads()
{
	return roads_;
}

Float World::GetHeight(Float x, Float y)
{
	return *heightMap_.Get(x * resolutionFactor_, y * resolutionFactor_);
}

Grid <Float> & World::GetHeightMap()
{
	return heightMap_;
}

void World::SetDetailResolution(int detailResolution)
{
	detailResolution_ = detailResolution;

	resolutionFactor_ = Float(detailResolution_);
}

WorldObject World::ProcessSelection(Camera* camera, Float3 to, Float2 mouse)
{
	collisionAttempts_.Reset();

	auto position = camera->GetPosition();
	auto rayDirection = glm::normalize(to - camera->GetPosition());

	while(true)
	{
		position += rayDirection * 0.1f;

		auto height = GetHeight(position.x, position.y);
		if(position.z < height - 10.0f)
			break;

		if(position.z < height - 0.5f || position.z > height + 0.5f)
			continue;

		auto tile = tiles_.Get(position.x, position.y);

		auto settlement = tile->GetSettlement();
		if(settlement == nullptr)
			continue;

		bool hasFound = false;
		for(auto attempt = collisionAttempts_.GetStart(); attempt != collisionAttempts_.GetEnd(); ++attempt)
		{
			if(attempt->Object_ != settlement)
				continue;

			hasFound = true;
			break;
		}

		if(hasFound)
			continue;

		bool isCollision = settlement->CheckCollision(camera->GetMatrix(), mouse);

		auto worldObject = WorldObject(settlement, WorldObjects::SETTLEMENT);
		*collisionAttempts_.Allocate() = worldObject;

		if(isCollision)
		{
			return worldObject;
		}
	}

	return WorldObject::DEFAULT;
}

void World::Update()
{
	if(isUpdating_ == false)
		return;

	UpdateSettlements();
}

void World::UpdateSettlements()
{
	for(auto settlement = settlements_.GetStart(); settlement != settlements_.GetEnd(); ++settlement)
	{
		settlement->Update();
	}
}
