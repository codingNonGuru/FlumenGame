#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenEngine/Utility/Perlin.hpp"
#include "FlumenEngine/Utility/Kernel.hpp"
#include "FlumenEngine/Render/Image.hpp"
#include "FlumenEngine/Render/StencilManager.hpp"

#include "FlumenGame/Generators/ReliefGenerator.hpp"
#include "FlumenGame/Generators/WorldGenerator.hpp"
#include "FlumenGame/Generators/WorldParameterSet.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Tile.hpp"
#include "FlumenGame/Types.hpp"

enum class Buffers
{
	TERRAIN, CARVE, GRADIENT, KERNEL, PARTICLE, PARTICLE_VELOCITY, PERLIN_DETAIL, HIGH_DETAIL_TERRAIN, STEPPE_DIFFUSE
};

Map <DataBuffer, Buffers> buffers = Map <DataBuffer, Buffers> (16);

Map <DataBuffer*> ReliefGenerator::modelBuffers_ = Map <DataBuffer*> (TerrainModelBuffers::COUNT);

Map <Texture*> ReliefGenerator::modelTextures_ = Map <Texture*> (TerrainModelTextures::COUNT);

Shader* shader = nullptr;

Size detailMapSize = Size(4096, 4096);

Size steppeTextureSize = Size(1024, 1024);

Grid <Float> detailMap = Grid <Float> (detailMapSize.x, detailMapSize.y);

const float ReliefGenerator::DETAIL_STRENGTH_MODIFIER = 150.0f;

const int ReliefGenerator::DETAIL_RESOLUTION = 16;

const int ReliefGenerator::DETAIL_TILE_COUNT = 4;

const float ReliefGenerator::DETAIL_STRENGTH = DETAIL_STRENGTH_MODIFIER / Float(DETAIL_TILE_COUNT);

const float ReliefGenerator::SEA_LEVEL = 0.0f;

void ReliefGenerator::Generate(World& world)
{
	Size size = world.GetSize();

	SetupBuffers(world);

	BindAssets(size);

	auto computeSize = size / 4;

	shader->SetConstant(0, "mode");
	shader->DispatchCompute(computeSize);

	float continentLift = size.x / 10;
	float islandLift = size.y / 20;

	Float2 position;

	position = (Float2)size * Float2(0.25f, 0.4f);
	LiftTerrain(position, continentLift, computeSize);
	position = (Float2)size * Float2(0.25f, 0.25f);
	LiftTerrain(position, continentLift, computeSize);

	position = (Float2)size * Float2(0.75f, 0.6f);
	LiftTerrain(position, continentLift, computeSize);
	position = (Float2)size * Float2(0.75f, 0.75f);
	LiftTerrain(position, continentLift, computeSize);

	position = (Float2)size * Float2(0.6f, 0.25f);
	LiftTerrain(position, islandLift, computeSize);

	position = (Float2)size * Float2(0.4f, 0.75f);
	LiftTerrain(position, islandLift, computeSize);

	for(int i = 0; i < 30; ++i)
	{
		shader->SetConstant(2, "mode");
		shader->DispatchCompute(computeSize);
	}

	/*Perlin::GetResultBuffer()->Bind(1);

	for(int i = 0; i < 0; ++i)
	{
		shader->SetConstant(2, "mode");
		shader->DispatchCompute(computeSize);
	}*/

	shader->SetConstant(3, "mode");
	shader->DispatchCompute(computeSize);

	shader->Unbind();

	/*for(int erodePass = 0; erodePass < 1; ++erodePass)
	  {
		glUniform1ui(0, 4);
		glDispatchCompute(width_ / workGroupSize, height_ / workGroupSize, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		for(int flowPass = 0; flowPass < 200; ++flowPass)
		{
			glUniform1ui(0, 10);
			glUniform2i(4, particles.getWidth(), particles.getHeight());
			glDispatchCompute(particles.getWidth(), particles.getHeight(), 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		glFinish();
	}*/

	Perlin::Generate(detailMapSize, FocusIndex(0.0f), ContrastThreshold(0.5f), ContrastStrength(1.0f));
	Perlin::Download(&detailMap);

	auto texture = new Texture(detailMapSize, TextureFormats::ONE_FLOAT, &detailMap);
	*modelTextures_.Add(TerrainModelTextures::DETAIL_HEIGHT) = texture;

	BindAssets(size);

	auto highDetailTerrainSize = size * DETAIL_RESOLUTION;
	shader->SetConstant(highDetailTerrainSize, "highDetailTerrainSize");

	shader->SetConstant(DETAIL_RESOLUTION, "detailResolution");

	shader->SetConstant(DETAIL_TILE_COUNT, "detailTileCount");

	shader->SetConstant(DETAIL_STRENGTH, "detailStrength");

	shader->BindTexture(texture, "reliefDetailMap");

	computeSize = highDetailTerrainSize / 4;

	shader->SetConstant(11, "mode");
	shader->DispatchCompute(computeSize);

	shader->Unbind();

	FillWorld(world);
}

DataBuffer* ReliefGenerator::GetFinalBuffer()
{
	return buffers.Get(Buffers::TERRAIN);
}

DataBuffer* ReliefGenerator::GetModelBuffer(Word identifier)
{
	auto bufferPointer = modelBuffers_.Get(identifier);
	if(bufferPointer == nullptr)
		return nullptr;

	return *bufferPointer;
}

Texture* ReliefGenerator::GetModelTexture(Word identifier)
{
	auto texturePointer = modelTextures_.Get(identifier);
	if(texturePointer == nullptr)
		return nullptr;

	return *texturePointer;
}

Grid <Float> * ReliefGenerator::GetHeightMap()
{
	return &detailMap;
}

void ReliefGenerator::FillWorld(World& world)
{
	Size size = world.GetSize();

	Grid <Float> terrain(size.x, size.y);
	buffers.Get(Buffers::TERRAIN)->Download(&terrain);

	world.SetDetailResolution(DETAIL_RESOLUTION);

	auto & highDetailTerrain = world.GetHeightMap();
	highDetailTerrain.Initialize(size.x * DETAIL_RESOLUTION, size.y * DETAIL_RESOLUTION);

	buffers.Get(Buffers::HIGH_DETAIL_TERRAIN)->Download(&highDetailTerrain);

	auto & tiles = world.GetTiles();
	tiles.Initialize(size.x, size.y);

	int count = 0;
	for(int x = 0; x < tiles.GetWidth(); ++x)
	{
		for(int y = 0; y < tiles.GetHeight(); ++y)
		{
			auto& position = tiles(x, y)->GetPosition();
			position.x = (float)x;
			position.y = (float)y;
			position.z = *terrain(x, y);

			int landCount = 0;
			Float heightSum = 0.0f;
			for(int i = 0; i < DETAIL_RESOLUTION; ++i)
			{
				for(int j = 0; j < DETAIL_RESOLUTION; ++j)
				{
					auto height = *highDetailTerrain.Get(x * DETAIL_RESOLUTION + i, y * DETAIL_RESOLUTION + j);
					heightSum += height;

					if(height > SEA_LEVEL)
						landCount++;
				}
			}

			auto landRatio = Float(landCount) / Float(DETAIL_RESOLUTION * DETAIL_RESOLUTION);
			tiles(x, y)->SetLandRatio(landRatio);

			auto averageHeight = Float(heightSum) / Float(DETAIL_RESOLUTION * DETAIL_RESOLUTION);
			tiles(x, y)->SetAverageHeight(averageHeight);

			count += landRatio > 0.5f ? 1 : 0;
		}
	}

	Grid <Byte4> previewData(size.x, size.y);
	for(int x = 0; x < previewData.GetWidth(); ++x)
	{
		for(int y = 0; y < previewData.GetHeight(); ++y)
		{
			auto previewPixel = previewData(x, y);
			auto height = tiles(x, y)->GetRelief() == ReliefTypes::LAND ? Byte(255) : Byte(0);
			*previewPixel = Byte4(128, 0, 0, height);
		}
	}

	auto previewTexture = WorldGenerator::GetReliefPreview();
	previewTexture->Upload(previewData.GetData());
}

void ReliefGenerator::SetupBuffers(World& world)
{
	auto size = world.GetSize();
	auto area = size.x * size.y;

	DataBuffer* buffer = nullptr;

	buffer = buffers.Add(Buffers::TERRAIN);
	if(buffer)
	{
		buffer->Generate(area * sizeof(Float));
	}

	buffer = buffers.Add(Buffers::CARVE);
	if(buffer)
	{
		buffer->Generate(area * sizeof(Float));
	}

	buffer = buffers.Add(Buffers::GRADIENT);
	if(buffer)
	{
		buffer->Generate(area * sizeof(Float) * 2);
	}

	auto kernel = new Kernel(7);
	kernel->Initialize(KernelTypes::GAUSS, 32.0f);

	buffer = buffers.Add(Buffers::KERNEL);
	if(buffer)
	{
		buffer->Generate(kernel->GetValues().GetMemorySize(), kernel->GetValues().GetStart());
	}

	GLuint finalBuffer, carveBuffer, gradientBuffer, filterBuffer, particleBuffer, particleVelocityBuffer;

	container::Grid <Float2> particles(size.x / 64, size.y / 64);
	for(int x = 0; x < particles.GetWidth(); ++x)
	{
		for(int y = 0; y < particles.GetHeight(); ++y)
		{
			float posX = float(x) * 64.0f + utility::GetRandom(16.0f, 48.0f);
			float posY = float(y) * 64.0f + utility::GetRandom(16.0f, 48.0f);

			*particles(x, y) = Float2(posX, posY);
		}
	}

	buffer = buffers.Add(Buffers::PARTICLE);
	if(buffer)
	{
		buffer->Generate(particles.GetMemorySize(), particles.GetStart());
	}

	container::Grid <Float2> particleVelocities(size.x, size.y);
	for(int x = 0; x < particleVelocities.GetWidth(); ++x)
		for(int y = 0; y < particleVelocities.GetHeight(); ++y)
		{
			*particleVelocities(x, y) = Float2(0.0f, 0.0f);
		}

	buffer = buffers.Add(Buffers::PARTICLE_VELOCITY);
	if(buffer)
	{
		buffer->Generate(particleVelocities.GetMemorySize(), particleVelocities.GetStart());
	}

	buffer = buffers.Add(Buffers::PERLIN_DETAIL);
	if(buffer)
	{
		buffer->Generate(area * sizeof(Float));

		Perlin::SetTargetBuffer(buffer);
		Perlin::Generate(size, FocusIndex(0.0f), ContrastThreshold(0.5f), ContrastStrength(8.0f));

		//Perlin::Generate(size, FocusIndex(0.0f), ContrastThreshold(0.5f), ContrastStrength(4.0f));
	}

	auto highDetailArea = area * DETAIL_RESOLUTION * DETAIL_RESOLUTION;
	buffer = buffers.Add(Buffers::HIGH_DETAIL_TERRAIN);
	if(buffer)
	{
		buffer->Generate(highDetailArea * sizeof(Float));
	}

	buffer = buffers.Add(Buffers::STEPPE_DIFFUSE);
	if(buffer)
	{
		buffer->Generate(steppeTextureSize.x * steppeTextureSize.y * sizeof(Float4));
	}

	shader = ShaderManager::GetShader("GenerateRelief");
}

void ReliefGenerator::BindAssets(Size size)
{
	if(!shader)
		return;

	shader->Bind();

	buffers.Get(Buffers::TERRAIN)->Bind(0);
	buffers.Get(Buffers::PERLIN_DETAIL)->Bind(1);
	buffers.Get(Buffers::CARVE)->Bind(2);
	buffers.Get(Buffers::GRADIENT)->Bind(9);
	buffers.Get(Buffers::KERNEL)->Bind(10);
	buffers.Get(Buffers::PARTICLE)->Bind(11);
	buffers.Get(Buffers::PARTICLE_VELOCITY)->Bind(12);
	buffers.Get(Buffers::HIGH_DETAIL_TERRAIN)->Bind(13);

	shader->SetConstant(size, "size");
}

void ReliefGenerator::LiftTerrain(Float2 position, Float decay, Size computeSize)
{
	shader->SetConstant(position, "target");
	shader->SetConstant(decay, "decay");

	shader->SetConstant(1, "mode");
	shader->DispatchCompute(computeSize);
}

void ReliefGenerator::GenerateModel(World& world)
{
	struct LinkBatch
	{
		Index links_[12];
		unsigned int count_;

		LinkBatch()
		{
			count_ = 0;
		}

		void Add(Index link)
		{
			links_[count_] = link;
			count_++;
		}
	};

	Index capacity = 4096 * 256;
	Array <Float3> vertexPositions(capacity);
	Array <Index> indices(16384 * 256);
	Array <unsigned int> indexSeries(16384);
	Array <LinkBatch> linkBatches(capacity);
	Array <unsigned int> links(capacity * 12);

	for(int i = 0; i < linkBatches.GetCapacity(); ++i)
		linkBatches.Allocate()->count_ = 0;

	float horIncr = 0.01f;
	float vertIncr = 0.866f * horIncr;

	bool flip = true;
	Index index = 0;

	*indexSeries.Allocate() = index;
	for(float y = -5.0f; y < 5.0f; y += vertIncr)
	{
		for(float x = -3.0f + (flip ? 0.0f : horIncr * 0.5f); x < 3.0f; x += horIncr)
		{
			*vertexPositions.Allocate() = Float3(x, y, 0.0f);
			index++;
		}
		flip = flip ? false : true;
		*indexSeries.Allocate() = index;
	}

	for(int i = 0; i < indexSeries.GetSize() - 3; i += 2)
	{
		bool first = false;
		int j = *indexSeries.Get(i);
		int l = *indexSeries.Get(i + 2);
		for(int k = *indexSeries.Get(i + 1); k < *indexSeries.Get(i + 2) - 3; ++j, ++k, ++l)
		{
			*indices.Allocate() = j;
			*indices.Allocate() = j + 1;
			*indices.Allocate() = k;

			linkBatches.Get(j)->Add(j + 1);
			linkBatches.Get(j)->Add(k);
			linkBatches.Get(j + 1)->Add(j);
			linkBatches.Get(j + 1)->Add(k);
			linkBatches.Get(k)->Add(j);
			linkBatches.Get(k)->Add(j + 1);

			*indices.Allocate() = j + 1;
			*indices.Allocate() = k;
			*indices.Allocate() = k + 1;

			linkBatches.Get(j + 1)->Add(k);
			linkBatches.Get(j + 1)->Add(k + 1);
			linkBatches.Get(k)->Add(j + 1);
			linkBatches.Get(k)->Add(k + 1);
			linkBatches.Get(k + 1)->Add(j + 1);
			linkBatches.Get(k + 1)->Add(k);

			*indices.Allocate() = l;
			*indices.Allocate() = k;
			*indices.Allocate() = l + 1;

			linkBatches.Get(l)->Add(k);
			linkBatches.Get(l)->Add(l + 1);
			linkBatches.Get(k)->Add(l);
			linkBatches.Get(k)->Add(l + 1);
			linkBatches.Get(l + 1)->Add(l);
			linkBatches.Get(l + 1)->Add(k);

			*indices.Allocate() = l + 1;
			*indices.Allocate() = k;
			*indices.Allocate() = k + 1;

			linkBatches.Get(l + 1)->Add(k);
			linkBatches.Get(l + 1)->Add(k + 1);
			linkBatches.Get(k)->Add(l + 1);
			linkBatches.Get(k)->Add(k + 1);
			linkBatches.Get(k + 1)->Add(l + l);
			linkBatches.Get(k + 1)->Add(k);
		}
	}

	for(int i = 0; i < linkBatches.GetSize(); ++i)
	{
		for(int j = 0; j < 12; ++j)
			*links.Allocate() = linkBatches.Get(i)->links_[j];
	}

	auto positionInputBuffer = modelBuffers_.Add(TerrainModelBuffers::POSITION_INPUT);
	*positionInputBuffer = new DataBuffer(vertexPositions.GetMemorySize(), vertexPositions.GetStart());
	(*positionInputBuffer)->SetSize(vertexPositions.GetSize());

	auto indexLinkBuffer = modelBuffers_.Add(TerrainModelBuffers::INDEX_LINKS);
	*indexLinkBuffer = new DataBuffer(links.GetMemorySize(), links.GetStart());

	auto indexBuffer = modelBuffers_.Add(TerrainModelBuffers::INDICES);
	*indexBuffer = new DataBuffer(indices.GetMemorySize(), indices.GetStart());
	(*indexBuffer)->SetSize(indices.GetSize());

	auto & worldTiles = world.GetTiles();
	Grid <Float> heightMap (worldTiles.GetWidth(), worldTiles.GetHeight());
	for(int x = 0; x < heightMap.GetWidth(); ++x)
		for(int y = 0; y < heightMap.GetHeight(); ++y)
		{
			//*heightMap(x, y) = (worldTiles.Get(x, y)->position_.z - world.averageHeight_) * 1.0f + world.averageHeight_;
			auto & position = worldTiles.Get(x, y)->GetPosition();
			*heightMap(x, y) = position.z;
		}

	auto texture = new Texture(world.GetSize(), TextureFormats::ONE_FLOAT, &heightMap);
	*modelTextures_.Add(TerrainModelTextures::BASE_HEIGHT) = texture;

	Size detailSize(4096, 4096);
	Perlin::Generate(detailSize, FocusIndex(0.7f), ContrastThreshold(0.5f), ContrastStrength(2.0f));

	texture = new Texture(detailSize, TextureFormats::ONE_FLOAT, *Perlin::GetResultBuffer());
	*modelTextures_.Add(TerrainModelTextures::ROAD_DETAIL) = texture;
}

void ReliefGenerator::GenerateSteppeTextures()
{
	/*auto image = new Image(steppeTextureSize, Color::RED, ImageFormats::RGB);

	Length passCount = 1024;
	for(Index pass = 0; pass < passCount; ++pass)
	{
		Index stencilIndex = utility::GetRandom(0, 15);
		auto stencil = StencilManager::Get("Paper", stencilIndex);

		auto stencilSize = stencil->GetSize();

		float alphaModifier = exp(-(float)pass / 700.0f);
		Float alpha = utility::GetRandom(0.2f, 0.3f) * alphaModifier + 0.02f;

		Size offset = Size(utility::GetRandom(0, steppeTextureSize.x) - stencilSize.x / 2, utility::GetRandom(0, steppeTextureSize.y) - stencilSize.y / 2);

		Color color = basePalette.GetColor();

		stencil->Apply(image, alpha, color, offset);
	}

	auto texture = new Texture(steppeTextureSize, TextureFormats::FOUR_FLOAT, image);
	*modelTextures_.Add(TerrainModelTextures::STEPPE_DIFFUSE) = texture;*/
}
