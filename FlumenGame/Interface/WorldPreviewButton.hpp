#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenGame/Types.hpp"

class WorldPreviewPanel;

class WorldPreviewButton : public Element
{
	WorldPreviewModes mode_;

	WorldPreviewPanel* previewPanel_;

	virtual void HandleInitialize() override;

public:
	void SetMode(WorldPreviewModes);

	void Press();
};
