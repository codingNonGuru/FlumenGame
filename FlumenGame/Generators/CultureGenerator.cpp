#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/Block.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenCore/Time.hpp"

#include "CultureGenerator.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Generators/WorldGenerator.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/Tile.hpp"
#include "FlumenGame/Road.hpp"
#include "FlumenGame/Economy.hpp"

#define MAXIMUM_SETTLEMENT_COUNT 65536

void CultureGenerator::Generate(World& world)
{
	GenerateSettlements(world);

	GenerateLinks(world);

	GenerateRoads(world);

	GenerateDomains(world);

	auto & tiles = world.GetTiles();

	auto size = world.GetSize();
	Grid <Byte4> textureData(size.x, size.y);
	for(int x = 0; x < size.x; ++x)
	{
		for(int y = 0; y < size.y; ++y)
		{
			auto tile = tiles(x, y);
			if(tile->GetRelief() == ReliefTypes::OCEAN)
			{
				*textureData(x, y) = Byte4(128, 0, 0, 0);
			}
			else
			{
				if(tile->GetSettlement() != nullptr)
				{
					*textureData(x, y) = Byte4(255, 255, 255, 255);
				}
				else
				{
					*textureData(x, y) = Byte4(128, 0, 0, 255);
				}
			}
			/*else
			{
				auto domain = tile->GetDomain();
				if(domain != nullptr)
				{
					auto banner = domain->GetBanner();
					*textureData(x, y) = Byte4(banner.r_ * 255.0f, 0, 0, 255);
				}
				else
				{
					*textureData(x, y) = Byte4(128, 0, 0, 255);
				}
			}*/
		}
	}

	auto texture = WorldGenerator::GetCulturePreview();
	texture->Upload(textureData.GetStart());
}

void CultureGenerator::GenerateSettlements(World& world)
{
	Time::StartClock();

	auto & tiles = world.GetTiles();

	auto & settlements = world.GetSettlements();

	auto & economies = world.GetEconomies();

	settlements.Initialize(MAXIMUM_SETTLEMENT_COUNT);

	economies.Initialize(MAXIMUM_SETTLEMENT_COUNT);

	Settlement::SetWorld(&world);

	int globalPopulation = 0;
	int highestPopulation = 0;
	int lowestPopulation = 65536;
	for(int x = 0; x < tiles.GetWidth(); ++x)
	{
		for(int y = 0; y < tiles.GetHeight(); ++y)
		{
			auto tile = tiles(x, y);
			if(tile->GetLandRatio() < 0.1f)
				continue;

			auto & biome = tile->GetBiome();

			float chance = biome.Productivity_;
			if(chance < 0.1f)
				continue;

			bool hasFound = false;
			int range = 2;
			for(int i = x - range; i <= x + range; ++i)
			{
				for(int j = y - range; j <= y + range; ++j)
				{
					if(tiles(i, j)->GetSettlement() != nullptr)
					{
						hasFound = true;
						break;
					}
				}
			}

			if(hasFound == true)
				continue;

			chance *= 0.05f;

			if(utility::RollDice(chance) == false)
				continue;

			auto settlement = settlements.Allocate();
			if(settlement == nullptr)
				continue;

			Position2 position = Position2(x, y) + Position2(utility::GetRandom(0.2f, 0.8f), utility::GetRandom(0.2f, 0.8f));
			settlement->Initialize(position);

			auto key = settlement - settlements.GetStart();
			settlement->SetKey(key);

			tile->SetSettlement(settlement);

			globalPopulation += settlement->GetPopulation();

			if(settlement->GetPopulation() > highestPopulation)
				highestPopulation = settlement->GetPopulation();

			if(settlement->GetPopulation() < lowestPopulation)
				lowestPopulation = settlement->GetPopulation();
		}
	}

	auto time = Time::GetClock();
	std::cout<<"SETTLEMENT GENERATION TIME (in seconds) ------> "<<time<<"\n";

	std::cout<<"SETTLEMENT COUNT ----------> "<<settlements.GetSize()<<"\n";
	std::cout<<"GLOBAL POPULATION ----------> "<<globalPopulation<<"\n";
	std::cout<<"HIGHEST POPULATION ----------> "<<highestPopulation<<"\n";
	std::cout<<"LOWEST POPULATION ----------> "<<lowestPopulation<<"\n";
	std::cout<<"AVERAGE POPULATION ----------> "<<globalPopulation / settlements.GetSize()<<"\n";
}

#define DEFAULT_MINIMUM_DISTANCE 1000.0f

#define MAXIMUM_DISTANCE 25.0f

#define MINIMUM_ANGLE 3.1415f / 6.0f

struct LinkData;

Array <LinkData> linkDatas;

struct Neighbour
{
	Settlement* Other_;

	Float Distance_;

	bool IsLinked_;

	bool IsBlocked_;

	Neighbour() : Other_(nullptr) {}

	Neighbour(Settlement* Other, Float Distance) : Other_(Other), Distance_(Distance), IsLinked_(false), IsBlocked_(false) {}

	void Link() {IsLinked_ = true;}

	void Unlink() {IsLinked_ = false;}

	void Block() {IsBlocked_ = true;}
};

struct LinkData
{
	Settlement* Settlement_;

	container::Block <Neighbour, 128> Neighbours_;

	Length NeighbourCount_;

	Length LinkCount_;

	LinkData() {}

	LinkData(Settlement* Settlement) : Settlement_(Settlement)
	{
		for(auto neighbour = Neighbours_.GetStart(); neighbour != Neighbours_.GetEnd(); ++neighbour)
			*neighbour = Neighbour(nullptr, 0.0f);

		NeighbourCount_ = 0;

		LinkCount_ = 0;
	}

	void AddLink(Settlement* settlement)
	{
		for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
		{
			if(neighbour->Other_ == settlement)
			{
				neighbour->Link();
				LinkCount_++;
				break;
			}
		}
	}

	void RemoveLink(Settlement* settlement)
	{
		for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
		{
			if(neighbour->Other_ == settlement)
			{
				neighbour->Unlink();
				LinkCount_--;
				break;
			}
		}
	}

	void BlockLink(Settlement* settlement)
	{
		for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
		{
			if(neighbour->Other_ == settlement)
			{
				neighbour->Block();
				break;
			}
		}
	}

	void AddNeighbour(Neighbour neighbour)
	{
		*Neighbours_.Find(NeighbourCount_) = neighbour;

		NeighbourCount_++;

		for(Index pass = 0; pass < NeighbourCount_; ++pass)
		{
			for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
			{
				auto nextNeighbour = neighbour + 1;
				if(nextNeighbour->Other_ == nullptr)
					continue;

				if(neighbour->Distance_ > nextNeighbour->Distance_)
				{
					auto swapNeighbour = *neighbour;
					*neighbour = *nextNeighbour;
					*nextNeighbour = swapNeighbour;
				}
			}
		}
	}

	Neighbour* GetClosestNeighbour()
	{
		for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
		{
			if(neighbour->IsLinked_ || neighbour->IsBlocked_)
				continue;

			bool isValid = true;

			auto directionToClosest = neighbour->Other_->GetPosition() - Settlement_->GetPosition();
			directionToClosest = glm::normalize(directionToClosest);
			for(auto otherNeighbour = GetFirstNeighbour(); otherNeighbour != GetLastNeighbour(); ++otherNeighbour)
			{
				if(otherNeighbour == neighbour)
					continue;

				auto directionToLinked = otherNeighbour->Other_->GetPosition() - Settlement_->GetPosition();
				directionToLinked = glm::normalize(directionToLinked);

				auto angle = glm::dot(directionToLinked, directionToClosest);
				angle = acos(angle);

				if(otherNeighbour->IsLinked_)
				{
					if(angle < MINIMUM_ANGLE)
					{
						isValid = false;
					}
				}
			}

			if(isValid)
				return neighbour;
		}

		return nullptr;
	}

	Neighbour* GetFirstNeighbour()
	{
		return Neighbours_.GetStart();
	}

	Neighbour* GetLastNeighbour()
	{
		return Neighbours_.GetStart() + NeighbourCount_;
	}

	void BlockNeighbours()
	{
		for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
		{
			if(neighbour->IsLinked_ || neighbour->IsBlocked_)
				continue;

			auto directionToClosest = neighbour->Other_->GetPosition() - Settlement_->GetPosition();
			directionToClosest = glm::normalize(directionToClosest);
			for(auto otherNeighbour = GetFirstNeighbour(); otherNeighbour != GetLastNeighbour(); ++otherNeighbour)
			{
				if(otherNeighbour == neighbour)
					continue;

				if(!otherNeighbour->IsLinked_ && !otherNeighbour->IsBlocked_)
					continue;

				auto directionToLinked = otherNeighbour->Other_->GetPosition() - Settlement_->GetPosition();
				directionToLinked = glm::normalize(directionToLinked);

				auto angle = glm::dot(directionToLinked, directionToClosest);
				angle = acos(angle);

				if(angle > MINIMUM_ANGLE * (otherNeighbour->IsBlocked_ ? 0.5f : 1.0f))
					continue;

				if(otherNeighbour->IsBlocked_ && otherNeighbour->Distance_ > neighbour->Distance_)
					continue;

				BlockLink(neighbour->Other_);

				auto otherLinkData = linkDatas.GetStart() + neighbour->Other_->GetKey();
				otherLinkData->BlockLink(Settlement_);

				break;
			}
		}
	}

	void CleanUp()
	{
		for(auto neighbour = GetFirstNeighbour(); neighbour != GetLastNeighbour(); ++neighbour)
		{
			if(!neighbour->IsLinked_)
				continue;

			auto otherSettlement = neighbour->Other_;

			auto directionToClosest = otherSettlement->GetPosition() - Settlement_->GetPosition();
			directionToClosest = glm::normalize(directionToClosest);
			for(auto otherNeighbour = GetFirstNeighbour(); otherNeighbour != GetLastNeighbour(); ++otherNeighbour)
			{
				if(otherNeighbour == neighbour)
					continue;

				if(otherNeighbour->IsLinked_)
					continue;

				auto directionToLinked = otherNeighbour->Other_->GetPosition() - Settlement_->GetPosition();
				directionToLinked = glm::normalize(directionToLinked);

				auto angle = glm::dot(directionToLinked, directionToClosest);
				angle = acos(angle);

				if(angle > MINIMUM_ANGLE * 0.6f)
					continue;

				if(otherNeighbour->Distance_ > neighbour->Distance_)
					continue;

				RemoveLink(neighbour->Other_);

				auto otherLinkData = linkDatas.GetStart() + neighbour->Other_->GetKey();
				otherLinkData->RemoveLink(Settlement_);

				break;
			}
		}
	}
};

void CultureGenerator::GenerateLinks(World& world)
{
	auto & settlements = world.GetSettlements();
	auto & tiles = world.GetTiles();

	linkDatas.Initialize(settlements.GetSize());
	linkDatas.AllocateFully();

	int searchRange = 27;

	auto linkData = linkDatas.GetStart();
	for(auto settlement = settlements.GetStart(); settlement != settlements.GetEnd(); ++settlement, ++linkData)
	{
		*linkData = LinkData(settlement);

		auto position = settlement->GetPosition();
		for(int x = (int)position.x - searchRange; x <= (int)position.x + searchRange; ++x)
		{
			for(int y = (int)position.y - searchRange; y <= (int)position.y + searchRange; ++y)
			{
				auto tile = tiles(x, y);
				auto otherSettlement = tile->GetSettlement();
				if(otherSettlement == nullptr || otherSettlement == settlement)
					continue;

				float distance = settlement->GetDistance(otherSettlement);
				if(distance > MAXIMUM_DISTANCE)
					continue;

				linkData->AddNeighbour(Neighbour(otherSettlement, distance));
			}
		}
	}

	Array <Neighbour> candidates(64);

	Array <Length> linkCounts(256);

	Length linkCount = 0;
	while (true)
	{
		linkData = linkDatas.GetStart();
		for(auto settlement = settlements.GetStart(); settlement != settlements.GetEnd(); ++settlement, ++linkData)
		{
			auto closestNeighbour = linkData->GetClosestNeighbour();
			if(closestNeighbour == nullptr)
				continue;

			auto closestSettlement = closestNeighbour->Other_;
			auto neighbourLinkData = linkDatas.GetStart() + closestSettlement->GetKey();

			auto neighboursClosestNeighbour = neighbourLinkData->GetClosestNeighbour();

			if(neighboursClosestNeighbour == nullptr)
				continue;

			if(neighboursClosestNeighbour->Other_ != settlement)
				continue;

			linkData->AddLink(closestSettlement);
			neighbourLinkData->AddLink(settlement);

			linkCount += 2;
		}

		for(Index blockPass = 0; blockPass < 4; ++blockPass)
		{
			for(linkData = linkDatas.GetStart(); linkData != linkDatas.GetEnd(); ++linkData)
			{
				linkData->BlockNeighbours();
			}
		}

		Length succesiveMatches = 0;
		for(auto count = linkCounts.GetEnd() - 1; count != linkCounts.GetStart(); --count)
		{
			if(*count == linkCount)
				succesiveMatches++;
			else
				break;
		}

		*linkCounts.Allocate() = linkCount;

		if(succesiveMatches > 0)
		{
			break;
		}
	}

	for(Index cleanPass = 0; cleanPass < 16; ++cleanPass)
	{
		for(linkData = linkDatas.GetStart(); linkData != linkDatas.GetEnd(); ++linkData)
		{
			linkData->CleanUp();
		}
	}

	linkCount = 0;
	for(linkData = linkDatas.GetStart(); linkData != linkDatas.GetEnd(); ++linkData)
	{
		linkCount += linkData->LinkCount_;
	}

	auto & links = world.GetLinks();
	links.Initialize(linkCount);

	linkData = linkDatas.GetStart();
	for(auto settlement = settlements.GetStart(); settlement != settlements.GetEnd(); ++settlement, ++linkData)
	{
		auto firstLink = links.GetEnd();
		settlement->SetLinkNetwork(LinkNetwork(firstLink, linkData->LinkCount_));
		for(auto neighbour = linkData->GetFirstNeighbour(); neighbour != linkData->GetLastNeighbour(); ++neighbour)
		{
			if(!neighbour->IsLinked_)
				continue;

			auto settlement = neighbour->Other_;
			*links.Allocate() = Link(settlement);
		}
	}

	std::cout<<"TOTAL NUMBER OF LINKS BETWEEN SETTLEMENTS --------> "<<linkCount<<"\n";
}

struct TileRoadData
{
	container::Block <Road, 8> Roads_;

	Length RoadCount_;

	TileRoadData()
	{
		RoadCount_ = 0;
	}

	void Add(Road road)
	{
		*Roads_.Find(RoadCount_) = road;

		RoadCount_++;
	}

	bool HasRoad(Settlement* first, Settlement* second)
	{
		for(auto road = Roads_.GetStart(); road != Roads_.GetStart() + RoadCount_; ++road)
		{
			if(road->Start_ == first && road->End_ == second)
				return true;

			if(road->Start_ == second && road->End_ == first)
				return true;
		}

		return false;
	}
};

void CultureGenerator::GenerateRoads(World& world)
{
	auto & tiles = world.GetTiles();

	Grid <TileRoadData> roadDatas(tiles.GetWidth(), tiles.GetHeight());

	Length roadCount = 0;

	auto & settlements = world.GetSettlements();
	for(auto settlement = settlements.GetStart(); settlement != settlements.GetEnd(); ++settlement)
	{
		auto linkNetwork = settlement->GetLinkNetwork();
		for(auto link = linkNetwork.GetFirst(); link != linkNetwork.GetLast(); ++link)
		{
			auto middlePoint = settlement->GetPosition() * 0.5f + link->Other_->GetPosition() * 0.5f;

			auto roadData = roadDatas.Get(middlePoint.x, middlePoint.y);
			if(roadData->HasRoad(settlement, link->Other_))
				continue;

			roadData->Add(Road(settlement, link->Other_));
			roadCount++;
		}
	}

	auto & roads = world.GetRoads();
	roads.Initialize(roadCount);

	auto tileRoadData = roadDatas.GetStart();
	for(auto tile = tiles.GetStart(); tile != tiles.GetEnd(); ++tile, ++tileRoadData)
	{
		tile->SetRoads(RoadData(roads.GetEnd(), tileRoadData->RoadCount_));
		for(auto road = tileRoadData->Roads_.GetStart(); road != tileRoadData->Roads_.GetStart() + tileRoadData->RoadCount_; ++road)
		{
			*roads.Allocate() = *road;
		}
	}
}

void CultureGenerator::GenerateDomains(World& world)
{
	Time::StartClock();

	auto & tiles = world.GetTiles();

	auto & settlements = world.GetSettlements();

	int searchRange = 20;

	for(auto tile = tiles.GetStart(); tile != tiles.GetEnd(); ++tile)
	{
		if(tile->GetRelief() == ReliefTypes::OCEAN)
			continue;

		float minimumDistance = DEFAULT_MINIMUM_DISTANCE;
		Settlement* closestSettlement = nullptr;

		auto basePosition = tile->GetPosition();
		for(int x = (int)basePosition.x - searchRange; x <= (int)basePosition.x + searchRange; ++x)
		{
			for(int y = (int)basePosition.y - searchRange; y <= (int)basePosition.y + searchRange; ++y)
			{
				auto otherTile = tiles(x, y);

				auto otherSettlement = otherTile->GetSettlement();
				if(otherSettlement == nullptr)
					continue;

				float distance = glm::length(otherTile->GetPosition() - tile->GetPosition());
				if(distance > minimumDistance)
					continue;

				minimumDistance = distance;
				closestSettlement = otherSettlement;
			}
		}

		if(closestSettlement == nullptr)
			continue;

		tile->SetDomain(closestSettlement);
	}

	auto time = Time::GetClock();
	std::cout<<"DOMAIN GENERATION TIME ------> "<<time<<" seconds\n";
}
