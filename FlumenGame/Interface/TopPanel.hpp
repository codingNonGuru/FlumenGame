#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class World;

class TopPanel : public Element
{
	Text* developmentLabel_;

	Text* potentialLabel_;

	World* world_;

protected:
	virtual void HandleInitialize() override;

	virtual void HandleUpdate() override;
};
