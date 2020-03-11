#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenGame/Types.hpp"

class World;
struct WorldParameterSet;
class Texture;

class WorldGenerator
{
	static const WorldParameterSet* parameterSet_;

	static Map <Texture*, WorldSizeOptions> reliefTextures_;

	static Map <Texture*, WorldSizeOptions> biomeTextures_;

	static Map <Texture*, WorldSizeOptions> cultureTextures_;

public:
	static Delegate OnWorldGenerated_;

	static Delegate OnReliefGenerated_;

	static Delegate OnBiomeGenerated_;

	static void Generate(World&, const WorldParameterSet&);

	static Texture* GetReliefPreview();

	static Texture* GetBiomePreview();

	static Texture* GetCulturePreview();

	static Size GetSize(WorldSizeOptions);
};
