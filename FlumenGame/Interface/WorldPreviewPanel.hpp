#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenGame/Types.hpp"

class WorldPreviewPanel : public Element
{
	Element* previewImage_;

	virtual void HandleInitialize() override;

public:
	void RefreshImage();

	void SetViewMode(WorldPreviewModes);
};
