#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "BottomInfoPanel.hpp"
#include "InterfaceBuilder.hpp"
#include "FlumenGame/Scenes/WorldScene.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/SettlementRenderer.hpp"
#include "FlumenGame/Economy.hpp"

void BottomInfoPanel::HandleInitialize()
{
	auto font = FontManager::GetFont("Dominican");
	auto textColor = InterfaceBuilder::GetTextColor();

	populationLabel_ = (Text*)GetChild("PopulationLabel");

	buildingLabel_ = (Text*)GetChild("BuildingLabel");

	developmentLabel_ = (Text*)GetChild("DevelopmentLabel");

	technologyLabel_ = Interface::AddElement("TechnologyLabel", new Text(font, textColor));
	technologyLabel_->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(0.0f, 150.0f)), nullptr);

	technologyLabel_->Enable();
	technologyLabel_->SetParent(this);

	resourceLabel_ = Interface::AddElement("PotentialLabel", new Text(font, textColor));
	resourceLabel_->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(0.0f, 200.0f)), nullptr);

	resourceLabel_->Enable();
	resourceLabel_->SetParent(this);
}

Word populationString;
Word buildingString;
Word developmentString;
Word technologyString;
Word resourceString;

void BottomInfoPanel::HandleUpdate()
{
	if(populationLabel_ == nullptr)
		return;

	auto selection = WorldScene::GetSelectedObject();
	if(selection->Object_ == nullptr)
		return;

	auto settlement = (Settlement*)selection->Object_;

	auto population = settlement->GetPopulation();

	populationString = Word("Population: ");
	sprintf(populationString.GetEnd(), "%d", population);
	populationLabel_->Setup(populationString, 0.7f);

	auto buildingCount = settlement->GetRenderData()->BuildingCount_;

	buildingString = Word("Buildings: ");
	sprintf(buildingString.GetEnd(), "%d", buildingCount);
	buildingLabel_->Setup(buildingString, 0.5f);

	const Economy& economy = settlement->GetEconomy();

	developmentString = Word("Development: ");
	sprintf(developmentString.GetEnd(), "%d", int(economy.GetDevelopment() * 100.0f));
	developmentLabel_->Setup(developmentString, 0.5f);

	technologyString = Word("Technology: ");
	sprintf(technologyString .GetEnd(), "%f", economy.GetTechnology());
	technologyLabel_->Setup(technologyString, 0.5f);

	resourceString = Word("Resource: ");
	sprintf(resourceString .GetEnd(), "%d", int(economy.GetPotential() * 100.0f));
	resourceLabel_->Setup(resourceString, 0.5f);
}
