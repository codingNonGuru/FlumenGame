#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class CloseButton : public Element
{
	using Element::Element;

private:
	float opacityFactor_;

public:
	virtual void HandleConfigure();

	virtual void HandleUpdate();
};
