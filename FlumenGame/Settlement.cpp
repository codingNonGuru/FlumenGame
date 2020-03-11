#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenEngine/Render/Mesh.hpp"

#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Tile.hpp"
#include "FlumenGame/SettlementRenderer.hpp"
#include "FlumenGame/Economy.hpp"

World* Settlement::world_ = nullptr;

void Settlement::SetWorld(World* world)
{
	world_ = world;
}

Settlement::Settlement()
{
	tile_ = nullptr;

	banner_ = Color(utility::GetRandom(0.0f, 1.0f), 0.0f, 0.0f);

	renderData_ = nullptr;
}

void Settlement::Initialize(Position2 position)
{
	position_ = position;

	auto & tiles = world_->GetTiles();
	tile_ = tiles(position_.x, position_.y);

	longitude_ = (int)position_.x;
	latitude_ = (int)position_.y;

	auto & economies = world_->GetEconomies();
	economy_ = economies.Allocate();
	*economy_ = Economy(this);
}

#define MAXIMUM_VERTEX_COUNT 16384

bool Settlement::CheckCollision(Matrix & matrix, Float2 mouse)
{
	Float4 positions[MAXIMUM_VERTEX_COUNT];

	int positionCount = 0;
	for(auto building = renderData_->Buildings_; building != renderData_->Buildings_ + renderData_->BuildingCount_; ++building)
	{
		auto mesh = SettlementRenderer::GetMesh(building->MeshIndex_);

		auto rotation = building->Rotation_;
		float s = sin(rotation);
		float c = cos(rotation);

		auto indexAttribute = mesh->GetAttribute("index");
		auto positionAttribute = mesh->GetAttribute("position");

		auto indexData = indexAttribute->GetData();
		auto positionData = positionAttribute->GetData();

		for(Index* indexIterator = indexData->GetData(); indexIterator != (Index*)indexData->GetData() + indexAttribute->GetSize(); ++indexIterator, ++positionCount)
		{
			auto index = *indexIterator;
			Float3* position = positionData->GetData() + index;

			Float3 rotatedPosition;
			rotatedPosition.x = position->x * c - position->y * s;
			rotatedPosition.y = position->x * s + position->y * c;
			rotatedPosition.z = position->z;

			positions[positionCount] = matrix * Float4(building->Position_ + rotatedPosition, 1.0f);
		}
	}

	Float2 screenPositions[MAXIMUM_VERTEX_COUNT];
	for(int i = 0; i < positionCount; ++i)
	{
		screenPositions[i].x = positions[i].x / positions[i].w;
		screenPositions[i].y = positions[i].y / positions[i].w;
	}

	for(int i = 0; i < positionCount; i += 3)
	{
		if(utility::IsInsideTriangle(mouse, screenPositions[i], screenPositions[i + 1], screenPositions[i + 2]))
		{
			return true;
		}
	}

	return false;
}

int Settlement::GetPopulation() const
{
	return population_;
}

LinkNetwork Settlement::GetLinkNetwork() const
{
	return linkNetwork_;
}

void Settlement::SetLinkNetwork(LinkNetwork linkNetwork)
{
	linkNetwork_ = linkNetwork;
}

Position2 Settlement::GetPosition() const
{
	return position_;
}

Color & Settlement::GetBanner()
{
	return banner_;
}

SettlementRenderData* Settlement::GetRenderData()
{
	return renderData_;
}

void Settlement::SetRenderData(SettlementRenderData* renderData)
{
	renderData_ = renderData;
}

float Settlement::GetDistance(Settlement* settlement) const
{
	auto direction = position_ - settlement->GetPosition();
	return glm::length(direction);
}

void Settlement::Update()
{
	economy_->Update();
}
