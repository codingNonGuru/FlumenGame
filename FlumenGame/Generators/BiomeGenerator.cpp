#include "FlumenEngine/Render/ImageProcessor.hpp"
#include "FlumenCore/Conventions.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Utility/Perlin.hpp"
#include "FlumenCore/Time.hpp"

#include "FlumenGame/Generators/BiomeGenerator.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Generators/ReliefGenerator.hpp"
#include "FlumenGame/Generators/WorldGenerator.hpp"
#include "FlumenGame/Tile.hpp"

DataBuffer* reliefBuffer = nullptr;

void BiomeGenerator::Generate(World& world)
{
	Time::StartClock();

	auto size = world.GetSize();

	auto& tiles = world.GetTiles();
	Grid <Float> reliefs(size.x, size.y);
	for(int x = 0; x < size.x; ++x)
	{
		for(int y = 0; y < size.y; ++y)
		{
			auto tile = tiles(x, y);
			*reliefs(x, y) = 1.0f - tile->GetLandRatio();
		}
	}

	if(reliefBuffer == nullptr)
	{
		reliefBuffer = new DataBuffer(reliefs.GetMemoryCapacity(), reliefs.GetStart());
	}
	else
	{
		reliefBuffer->UploadData(reliefs.GetStart(), reliefs.GetMemoryCapacity());
	}

	auto blurStrength = size.x / 4;
	auto output = ImageProcessor::Blur(reliefBuffer, size, blurStrength);

	auto variation = Perlin::Generate(size, FocusIndex(0.3f), ContrastThreshold(0.5f), ContrastStrength(2.0f));

	auto shader = ShaderManager::GetShader("GenerateBiome");
	if(shader == nullptr)
		return;

	shader->Bind();

	output->Bind(0);
	variation->Bind(1);

	shader->SetConstant(size, "size");

	shader->SetConstant(0, "stage");
	shader->DispatchCompute(size / 16);

	shader->SetConstant(1, "stage");
	shader->DispatchCompute(size / 16);

	shader->Unbind();

	auto time = Time::GetClock();
	std::cout<<"BIOME GENERATION TIME (in seconds) ------> "<<time<<"\n";

	Grid <Float> biomes(&reliefs);
	output->Download(&biomes);

	Grid <Byte4> textureData(size.x, size.y);
	for(int x = 0; x < size.x; ++x)
	{
		for(int y = 0; y < size.y; ++y)
		{
			Byte intensity;
			if(*biomes(x, y) > 0.75f)
				intensity = 64;
			else if(*biomes(x, y) > 0.5f)
				intensity = 128;
			else if(*biomes(x, y) > 0.25f)
				intensity = 192;
			else
				intensity = 255;

			*textureData(x, y) = Byte4(intensity, 0, 0, tiles(x, y)->GetRelief() == ReliefTypes::OCEAN ? 0 : 255);
			//*textureData(x, y) = Byte4(128, 0, 0, 255.0f * *biomes(x, y));
		}
	}

	auto texture = WorldGenerator::GetBiomePreview();
	texture->Upload(textureData.GetStart());

	for(int x = 0; x < size.x; ++x)
	{
		for(int y = 0; y < size.y; ++y)
		{
			auto tile = tiles(x, y);
			auto& biome = tile->GetBiome();

			biome.Productivity_ = *biomes(x, y);
		}
	}
}

void BiomeGenerator::SetupBuffers(World& world)
{

}
