#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenEngine/Utility/Perlin.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"

#include "FlumenGame/Generators/CultureModelBuilder.hpp"
#include "FlumenGame/SettlementRenderer.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/Types.hpp"
#include "FlumenGame/Road.hpp"

void CultureModelBuilder::Generate(World& world)
{
	Grid <Float> buildingMap (64, 64);

	auto noiseMapSize = Size(4096, 4096);
	Grid <Float> noiseMap(noiseMapSize.x, noiseMapSize.y);
	Perlin::Generate(noiseMapSize, FocusIndex(0.9f), ContrastThreshold(0.5f), ContrastStrength(4.0f));
	Perlin::Download(&noiseMap);

	auto & settlements = world.GetSettlements();

	auto capacity = settlements.GetSize() * 200;
	auto & buildingDatas = SettlementRenderer::GetBuildingDatas();
	buildingDatas.Initialize(capacity);

	auto & settlementDatas = SettlementRenderer::GetSettlementDatas();
	settlementDatas.Initialize(settlements.GetSize());

	Size offsetLimit = Size(noiseMapSize.x - buildingMap.GetWidth(), noiseMapSize.y - buildingMap.GetHeight());
	Index buildingIndex = 0;
	for(auto settlement = settlements.GetStart(); settlement != settlements.GetEnd(); ++settlement)
	{
		Size offset = Size(utility::GetRandom(0, offsetLimit.x), utility::GetRandom(0, offsetLimit.y));

		auto startIndex = buildingIndex;
		auto buildingCount = 0;

		const int halfSize = buildingMap.GetWidth() / 2;
		for(int x = 0; x < buildingMap.GetWidth(); ++x)
		{
			for(int y = 0; y < buildingMap.GetHeight(); ++y)
			{
				float populationFactor = (Float)settlement->GetPopulation() / 20.0f;

				float distanceFactor = float(x - halfSize) * float(x - halfSize) + float(y - halfSize) * float(y - halfSize);
				float heightFactor = distanceFactor;
				distanceFactor = exp(-distanceFactor / (2.0f * populationFactor));

				float chance = *noiseMap.Get(offset.x + x, offset.y + y);
				chance = (chance * 0.5f + distanceFactor * 0.5f) * distanceFactor;

				if(chance < 0.5f)
					continue;

				BuildingRenderData* buildingData = buildingDatas.Allocate();

				Float finalX = (Float(x) + (y % 2 == 0 ? 0.3f : 0.0f) - Float(halfSize)) * 0.2f;
				Float finalY = (Float(y) + (x % 2 == 0 ? 0.3f : 0.0f) - Float(halfSize)) * 0.2f;
				float scatterFactor = pow(2.0f - distanceFactor, 1.7f) * 0.85f;
				finalX *= scatterFactor;
				finalY *= scatterFactor;

				float angle = utility::GetRandom(0.0f, 6.2831f);
				float radius = utility::GetRandom(0.0f, 0.04f);
				finalX += cos(angle) * radius * scatterFactor;
				finalY += sin(angle) * radius * scatterFactor;

				buildingData->Position_ = Float3(settlement->GetPosition(), 0.0f) + Float3(finalX, finalY, 0.0f);
				buildingData->Position_.z = world.GetHeight(buildingData->Position_.x, buildingData->Position_.y);

				buildingData->Rotation_ = utility::GetRandom(0.0f, 6.2831f);

				heightFactor = exp(-heightFactor / (0.5f * populationFactor));
				buildingData->MeshIndex_ = heightFactor * 3.5f;
				if(buildingData->MeshIndex_ == 0)
					buildingData->MeshIndex_ = utility::GetRandom(0, 2);
				else if(buildingData->MeshIndex_ == 1)
					buildingData->MeshIndex_ = utility::GetRandom(3, 4);
				else
					buildingData->MeshIndex_ += 3;

				buildingData->SettlementIndex_ = settlement->GetKey();

				buildingIndex++;
				buildingCount++;
			}
		}

		auto settlementData = settlementDatas.Allocate();
		settlementData->BuildingIndex_ = startIndex;
		settlementData->BuildingCount_ = buildingCount;
		settlementData->Buildings_ = buildingDatas.GetStart() + startIndex;

		settlement->SetRenderData(settlementData);
	}

	auto & modelBuffers = SettlementRenderer::GetBuffers();

	auto buffer = modelBuffers.Add(SettlementModelBuffers::BUILDING_DATAS);
	*buffer = new DataBuffer(buildingDatas.GetMemorySize(), buildingDatas.GetStart());

	GenerateRoads(world);
}

void CultureModelBuilder::GenerateRoads(World& world)
{
	auto & roads = world.GetRoads();

	auto & linkDatas = SettlementRenderer::GetLinkDatas();
	linkDatas.Initialize(roads.GetSize());

	for(auto road = roads.GetStart(); road != roads.GetEnd(); ++road)
	{
		auto position = road->Start_->GetPosition() * 0.5f + road->End_->GetPosition() * 0.5f;

		auto direction = road->Start_->GetPosition() - road->End_->GetPosition();
		auto rotation = atan2(direction.y, direction.x);

		auto length = road->Start_->GetDistance(road->End_);

		auto textureIndex = utility::GetRandom(0, 15);

		*linkDatas.Allocate() = LinkRenderData(position, rotation, length, textureIndex);
	}

	auto & modelBuffers = SettlementRenderer::GetBuffers();

	auto buffer = modelBuffers.Add(SettlementModelBuffers::LINK_DATAS);
	*buffer = new DataBuffer(linkDatas.GetMemorySize(), linkDatas.GetStart());

	GenerateTextures(world);
}

void CultureModelBuilder::GenerateTextures(World& world)
{
	Size textureSize = Size(2048, 2048);

	Grid <Float> allRoadAlphas(textureSize.x, textureSize.y);
	Grid <Float> roadAlpha(textureSize.x, 128);
	Grid <Float> roadAlphaBuffer(textureSize.x, 128);
	Grid <Float> distortionAngles(textureSize.x, textureSize.y);
	//Grid <Float> distortionRanges(textureSize.x, textureSize.y);

	Perlin::Generate(textureSize, 0.0f, 0.5f, 1.0f);
	Perlin::Download(&distortionAngles);

	//Perlin::Generate(textureSize, 0.15f, 0.5f, 1.0f);
	//Perlin::Download(&distortionRanges);

	for(Index textureIndex = 0; textureIndex < 16; ++textureIndex)
	{
		int roadWidth = roadAlpha.GetHeight();
		for(int x = 0; x < roadAlpha.GetWidth(); ++x)
			for(int y = 0; y < roadAlpha.GetHeight(); ++y)
			{
				*roadAlpha(x, y) = exp(-pow(float(y - roadWidth / 2), 2.0f) / 256.0f);
				*roadAlphaBuffer(x, y) = 0.0f;
			}

		auto verticalOffset = textureIndex * roadAlpha.GetHeight();

		const float DIRECTION_MODIFIER = 150.0f;
		for(int x = 0; x < roadAlpha.GetWidth(); ++x)
			for(int y = 0; y < roadAlpha.GetHeight(); ++y)
			{
				float angle = *distortionAngles(x, y + verticalOffset) * 6.2831f;
				Float2 direction(cos(angle), sin(angle));
				float range = 1.0f - abs(glm::dot(direction, Float2(1.0f, 0.0f)));
				direction *= DIRECTION_MODIFIER;

				Float2 from(float(x) + direction.x, float(y) + direction.y);
				float topLeft = *roadAlpha(from.x, from.y);
				float topRight = *roadAlpha(from.x + 1.0f, from.y);
				float bottomLeft = *roadAlpha(from.x, from.y + 1.0f);
				float bottomRight = *roadAlpha(from.x + 1.0f, from.y + 1.0f);

				Float2 dif(abs(float(int(from.x)) - from.x), abs(float(int(from.y)) - from.y));
				*roadAlphaBuffer(x, y) += (1.0f - dif.x) * (1.0f - dif.y) * topLeft + dif.x * (1.0f - dif.y) * topRight + (1.0f - dif.x) * dif.y * bottomLeft + dif.x * dif.y * bottomRight;
			}

		for(int x = 0; x < roadAlphaBuffer.GetWidth(); ++x)
			for(int y = 0; y < roadAlphaBuffer.GetHeight(); ++y)
				*allRoadAlphas(x, y + verticalOffset) = *roadAlphaBuffer(x, y);
	}

	auto texture = new Texture(textureSize, TextureFormats::ONE_FLOAT, &allRoadAlphas);
	TextureManager::AddTexture(texture, "RoadAlpha");
}
