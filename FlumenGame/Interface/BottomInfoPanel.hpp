#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

class BottomInfoPanel : public Element
{
	Text* populationLabel_;

	Text* buildingLabel_;

	Text* developmentLabel_;

	Text* technologyLabel_;

	Text* resourceLabel_;

protected:
	virtual void HandleInitialize() override;

	virtual void HandleUpdate() override;
};
