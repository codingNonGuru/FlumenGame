#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"

#include "WorldPreviewPanel.hpp"
#include "FlumenGame/Generators/WorldGenerator.hpp"

void WorldPreviewPanel::HandleInitialize()
{
	previewImage_ = GetChild("PreviewImage");

	WorldGenerator::OnWorldGenerated_.Add(this, &WorldPreviewPanel::RefreshImage);
}

void WorldPreviewPanel::RefreshImage()
{
	SetViewMode(WorldPreviewModes::RELIEF);
}

void WorldPreviewPanel::SetViewMode(WorldPreviewModes mode)
{
	auto sprite = previewImage_->GetSprite();
	Texture* texture = nullptr;

	switch(mode)
	{
	case WorldPreviewModes::RELIEF:
		texture = WorldGenerator::GetReliefPreview();
		break;
	case WorldPreviewModes::BIOME:
		texture = WorldGenerator::GetBiomePreview();
		break;
	case WorldPreviewModes::POLITY:
		texture = WorldGenerator::GetCulturePreview();
		break;
	}

	sprite->SetTexture(texture);
}

