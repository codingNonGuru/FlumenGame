#pragma once

#include "FlumenCore/Conventions.hpp"

class World;
struct WorldParameterSet;
class DataBuffer;

class ReliefGenerator
{
	static Map <DataBuffer*> modelBuffers_;

	static Map <Texture*> modelTextures_;

	static void SetupBuffers(World&);

	static void LiftTerrain(Float2, Float, Size);

	static void BindAssets(Size);

	static void FillWorld(World&);

	static void GenerateSteppeTextures();

	static const float DETAIL_STRENGTH_MODIFIER;

public:
	static void Generate(World&);

	static void GenerateModel(World&);

	static DataBuffer* GetFinalBuffer();

	static DataBuffer* GetModelBuffer(Word);

	static Texture* GetModelTexture(Word);

	static Grid <Float> * GetHeightMap();

	static const int DETAIL_RESOLUTION;

	static const int DETAIL_TILE_COUNT;

	static const float DETAIL_STRENGTH;

	static const float SEA_LEVEL;
};
