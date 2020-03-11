#include "FilterBuilder.hpp"

#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/AnimationProperty.hpp"
#include "FlumenEngine/Animation/AnimationKey.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/FilterManager.hpp"
#include "FlumenEngine/Render/Filter.hpp"

#include "FlumenGame/Filter/BlurFilter.hpp"
#include "FlumenGame/Filter/SaturateFilter.hpp"

void FilterBuilder::GenerateFilters()
{
	auto saturateEffectShader = ShaderManager::GetShaderMap().Get("SaturateEffect");
	auto screenTexture = TextureManager::GetTexture("Screen");

	auto filter = FilterManager::AddFilter("Saturate", new SaturateFilter());
	filter->Initialize(saturateEffectShader, screenTexture);

	auto blurEffectShader = ShaderManager::GetShader("BlurEffect");

	filter = FilterManager::AddFilter("Blur", new BlurFilter());
	filter->Initialize(blurEffectShader, screenTexture);
}
