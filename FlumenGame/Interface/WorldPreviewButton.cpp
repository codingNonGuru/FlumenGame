#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenCore/Delegate/Delegate.hpp"

#include "WorldPreviewButton.hpp"
#include "WorldPreviewPanel.hpp"

void WorldPreviewButton::HandleInitialize()
{
	previewPanel_ = (WorldPreviewPanel*)Interface::GetElement(Elements::WORLD_PREVIEW_PANEL);

	SetInteractivity(true);
	clickEvents_->Add(this, &WorldPreviewButton::Press);

	auto icon = GetChild("Icon");
	if(icon != nullptr)
	{
		Texture* texture = nullptr;

		switch(mode_)
		{
		case WorldPreviewModes::RELIEF:
			texture = TextureManager::GetTexture("ReliefIcon");
			break;
		case WorldPreviewModes::BIOME:
			texture = TextureManager::GetTexture("BiomeIcon");
			break;
		case WorldPreviewModes::POLITY:
			texture = TextureManager::GetTexture("ReliefIcon");
			break;
		}
		auto iconSprite = icon->GetSprite();

		iconSprite->SetTexture(texture);
	}
}

void WorldPreviewButton::SetMode(WorldPreviewModes mode)
{
	mode_ = mode;
}

void WorldPreviewButton::Press()
{
	if(previewPanel_ != nullptr)
	{
		previewPanel_->SetViewMode(mode_);
	}
}
