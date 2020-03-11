#include "InterfaceBuilder.hpp"

#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/AnimationProperty.hpp"
#include "FlumenEngine/Animation/AnimationKey.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"
#include "FlumenEngine/Interface/Text.hpp"

#include "MainMenu.hpp"
#include "NewGameMenu.hpp"
#include "NewWorldMenu.hpp"
#include "WorldPreviewPanel.hpp"
#include "WorldPreviewButton.hpp"
#include "BottomInfoPanel.hpp"
#include "CloseButton.hpp"
#include "FlumenGame/Interface/TopBar.hpp"
#include "FlumenGame/Interface/TopPanel.hpp"
#include "InterfacePainter.hpp"

Shader* spriteShader = nullptr;
Shader* shadowShader = nullptr;

Color textColor = Color(0.5f, 0.1f, 0.05f);

void InterfaceBuilder::GenerateInterface()
{
	auto glyphShader = ShaderManager::GetShaderMap().Get("Glyph");

	TextManager::Initialize(glyphShader);

	InterfacePainter::PaintInterface();

	spriteShader = ShaderManager::GetShaderMap().Get("Sprite");
	shadowShader = ShaderManager::GetShaderMap().Get("Shadow");

	GenerateMainMenu();
	
	GenerateNewGameMenu();

	GenerateNewWorldMenu();

	GenerateWorldPreview();

	GenerateBottomInfoPanel();

	GenerateTopBar();

	GenerateTopPanel();

	auto elements = Interface::GetElements();
	for(auto elementIterator = elements.GetStart(); elementIterator != elements.GetEnd(); ++elementIterator)
	{
		auto element = *elementIterator;
		if(!element)
			continue;

		element->Initialize();
	}
}

Color& InterfaceBuilder::GetTextColor()
{
	return textColor;
}

void InterfaceBuilder::GenerateMainMenu()
{
	auto texture = TextureManager::GetTexture("MainMenu");
	auto sprite = new Sprite(texture, spriteShader);

	auto mainMenu = Interface::AddElement("MainMenu", new MainMenu());
	mainMenu->Configure(Size(1000, 600), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	auto animator = mainMenu->GetAnimator();

	AddOpenAnimation(mainMenu, 1800.0f, 0.0f, InterfaceElementParameters::POSITION_X);

	AddCloseAnimation(mainMenu, -1800.0f, 0.0f, InterfaceElementParameters::POSITION_X);

	texture = TextureManager::GetTexture("MainMenuShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(0), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(mainMenu);

	GenerateMainMenuNewGameButton();
}

void InterfaceBuilder::GenerateMainMenuNewGameButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 0);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("CloseButton", new CloseButton());
	button->Configure(Size(512, 256), DrawOrder(3), new Transform(Position2(0.0f, -300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto mainMenu = Interface::GetElement("MainMenu");
	button->SetParent(mainMenu);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(mainMenu, &Element::Close);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("Text", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("New Game");

	text->Enable();
	text->SetParent(button);
}

void InterfaceBuilder::GenerateNewGameMenu()
{
	auto texture = TextureManager::GetTexture("NewGameMenu");
	auto sprite = new Sprite(texture, spriteShader);

	auto menu = Interface::AddElement(Elements::NEW_GAME_MENU, new NewGameMenu());
	menu->Configure(Size(400, 400), DrawOrder(-1), new Transform(Position2(0.0f, 0.0f)), sprite);

	auto animator = menu->GetAnimator();

	AddOpenAnimation(menu, 1800.0f, 0.0f, InterfaceElementParameters::POSITION_X);

	AddCloseAnimation(menu, -1800.0f, 0.0f, InterfaceElementParameters::POSITION_X);

	texture = TextureManager::GetTexture("NewGameMenuShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(400, 400), DrawOrder(-2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(menu);

	GenerateNewWorldButton();

	GenerateNewGameMenuBackButton();
}

void InterfaceBuilder::GenerateNewWorldButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 0);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("NewWorldButton", new CloseButton());
	button->Configure(Size(512, 256), DrawOrder(3), new Transform(Position2(0.0f, -300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_GAME_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(parent, &NewGameMenu::OpenNewWorldMenu);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("Text", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("New World");

	text->Enable();
	text->SetParent(button);
}

void InterfaceBuilder::GenerateNewGameMenuBackButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 1);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("BackButton", new Element());
	button->Configure(Size(512, 256), DrawOrder(3), new Transform(Position2(0.0f, 300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_GAME_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(parent, &NewGameMenu::OpenMainMenu);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("Text", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("Back");

	text->Enable();
	text->SetParent(button);
}

void InterfaceBuilder::GenerateNewWorldMenu()
{
	auto texture = TextureManager::GetTexture("NewWorldMenu");
	auto sprite = new Sprite(texture, spriteShader);

	auto menu = Interface::AddElement(Elements::NEW_WORLD_MENU, new NewWorldMenu());
	menu->Configure(Size(400, 400), DrawOrder(-1), new Transform(Position2(0.0f, 0.0f)), sprite);

	AddOpenAnimation(menu, 1800.0f, 0.0f, InterfaceElementParameters::POSITION_X);

	AddCloseAnimation(menu, -1800.0f, 0.0f, InterfaceElementParameters::POSITION_X);

	texture = TextureManager::GetTexture("NewWorldMenuShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(400, 400), DrawOrder(-2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(menu);

	GenerateNewWorldMenuSizeButton();

	GenerateNewWorldMenuLeftScrollButton();

	GenerateNewWorldMenuRightScrollButton();

	GenerateNewWorldMenuGenerateButton();

	GenerateNewWorldMenuStartGameButton();

	GenerateNewWorldMenuBackButton();
}

void InterfaceBuilder::GenerateNewWorldMenuSizeButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 3);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("SizeButton", new Element());
	button->Configure(Size(512, 256), DrawOrder(3), new Transform(Position2(0.0f, -300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_WORLD_MENU);
	button->SetParent(parent);

	//button->SetInteractivity(true);
	//button->GetClickEvents().Add(parent, )

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("SizeLabel", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(100, 100), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("512 x 512");

	text->Enable();
	text->SetParent(button);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);
}

void InterfaceBuilder::GenerateNewWorldMenuLeftScrollButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::ROUND, ElementSizes::SMALL, 0);

	auto texture = textureSet->Base_;
	auto sprite = new Sprite(texture, spriteShader);

	auto button = Interface::AddElement("LeftScrollButton", new Element());
	button->Configure(Size(256, 256), DrawOrder(3), new Transform(Position2(-340.0f, -300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_WORLD_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(parent, &NewWorldMenu::ScrollLeftwards);

	texture = textureSet->Shadow_;
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	texture = TextureManager::GetTexture("Arrow");
	sprite = new Sprite(texture, spriteShader);

	auto icon = Interface::AddElement("ArrowIcon", new Element());
	icon->Configure(Size(256, 256), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	icon->Enable();
	icon->SetParent(button);
}

void InterfaceBuilder::GenerateNewWorldMenuRightScrollButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::ROUND, ElementSizes::SMALL, 1);

	auto texture = textureSet->Base_;
	auto sprite = new Sprite(texture, spriteShader);

	auto button = Interface::AddElement("RightScrollButton", new Element());
	button->Configure(Size(256, 256), DrawOrder(3), new Transform(Position2(340.0f, -300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_WORLD_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(parent, &NewWorldMenu::ScrollRightwards);

	texture = textureSet->Shadow_;
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	texture = TextureManager::GetTexture("RightArrow");
	sprite = new Sprite(texture, spriteShader);

	auto icon = Interface::AddElement("ArrowIcon", new Element());
	icon->Configure(Size(256, 256), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	icon->Enable();
	icon->SetParent(button);
}

void InterfaceBuilder::GenerateNewWorldMenuGenerateButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 4);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("GenerateButton", new Element());
	button->Configure(Size(256, 256), DrawOrder(3), new Transform(Position2(0.0f, -100.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_WORLD_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(parent, &NewWorldMenu::GenerateWorld);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("Text", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(100, 100), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("Generate");

	text->Enable();
	text->SetParent(button);
}

void InterfaceBuilder::GenerateNewWorldMenuStartGameButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 5);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("StartGameButton", new Element());
	button->Configure(Size(256, 256), DrawOrder(3), new Transform(Position2(0.0f, -300.0f)), sprite, Opacity(1.0f));

	auto parent = Interface::GetElement(Elements::NEW_WORLD_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	//button->GetClickEvents().Add(parent, &NewWorldMenu::GenerateWorld);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("Text", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(100, 100), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("Start Game");

	text->Enable();
	text->SetParent(button);
}

void InterfaceBuilder::GenerateNewWorldMenuBackButton()
{
	auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::SQUARE, ElementSizes::SMALL, 7);

	auto sprite = new Sprite(textureSet->Base_, spriteShader);

	auto button = Interface::AddElement("BackButton", new Element());
	button->Configure(Size(256, 256), DrawOrder(3), new Transform(Position2(0.0f, 300.0f)), sprite, Opacity(1.0f));

	button->Enable();
	auto parent = Interface::GetElement(Elements::NEW_WORLD_MENU);
	button->SetParent(parent);

	button->SetInteractivity(true);
	button->GetClickEvents().Add(parent, &NewWorldMenu::OpenNewGameMenu);

	sprite = new Sprite(textureSet->Shadow_, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(button);

	auto font = FontManager::GetFont("Dominican");
	auto text = Interface::AddElement("Text", new Text(font, Color(0.5f, 0.1f, 0.05f)));
	text->Configure(Size(100, 100), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
	((Text*)text)->Setup("Back");

	text->Enable();
	text->SetParent(button);
}

void InterfaceBuilder::GenerateWorldPreview()
{
	auto texture = TextureManager::GetTexture("WorldPreviewPanel");
	auto sprite = new Sprite(texture, spriteShader);

	auto panel = Interface::AddElement(Elements::WORLD_PREVIEW_PANEL, new WorldPreviewPanel());
	panel->Configure(Size(256, 256), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	AddOpenAnimation(panel, 1800.0f, 800.0f, InterfaceElementParameters::POSITION_X);

	AddCloseAnimation(panel, 1800.0f, 800.0f, InterfaceElementParameters::POSITION_X);

	texture = TextureManager::GetTexture("WorldPreviewPanelShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(0), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(panel);

	texture = nullptr;
	sprite = new Sprite(texture, spriteShader);

	auto image = Interface::AddElement("PreviewImage", new Element());
	image->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	image->Enable();
	image->SetParent(panel);

	GenerateWorldPreviewViewModeButtons();
}

void InterfaceBuilder::GenerateWorldPreviewViewModeButtons()
{
	Word buttonNames[] = {"ReliefModeButton", "BiomeModeButton", "PolityModeButton"};
	WorldPreviewModes previewModes[] = {WorldPreviewModes::RELIEF, WorldPreviewModes::BIOME, WorldPreviewModes::POLITY};

	for(int index = 0; index < 3; ++index)
	{
		auto textureSet = InterfacePainter::GetTextureSet(ElementShapes::ROUND, ElementSizes::SMALL, index);

		auto texture = textureSet->Base_;
		auto sprite = new Sprite(texture, spriteShader);

		auto button = new WorldPreviewButton();
		Interface::AddElement(buttonNames[index], button);
		float x = float(index - 1) * 200.0f;
		button->Configure(Size(256, 256), DrawOrder(4), new Transform(Position2(x, 330.0f)), sprite, Opacity(1.0f));

		button->Enable();
		auto panel = Interface::GetElement(Elements::WORLD_PREVIEW_PANEL);
		button->SetParent(panel);

		button->SetMode(previewModes[index]);

		texture = textureSet->Shadow_;
		sprite = new Sprite(texture, shadowShader);

		auto shadow = Interface::AddElement("Shadow", new Element());
		shadow->Configure(Size(150, 150), DrawOrder(3), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

		shadow->Enable();
		shadow->SetParent(button);

		sprite = new Sprite(nullptr, spriteShader);

		auto image = Interface::AddElement("Icon", new Element());
		image->Configure(Size(100, 100), DrawOrder(5), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

		image->Enable();
		image->SetParent(button);
	}
}

void InterfaceBuilder::GenerateBottomInfoPanel()
{
	auto texture = TextureManager::GetTexture("BottomInfoPanel");
	auto sprite = new Sprite(texture, spriteShader);

	auto panel = Interface::AddElement(Elements::BOTTOM_INFO_PANEL, new BottomInfoPanel());
	panel->Configure(Size(256, 256), DrawOrder(1), new Transform(Position2(-1044.0f, 420.0f)), sprite, Opacity(1.0f));

	AddOpenAnimation(panel, -1800.0f, -1044.0f, InterfaceElementParameters::POSITION_X);

	AddCloseAnimation(panel, -1800.0f, -1044.0f, InterfaceElementParameters::POSITION_X);

	texture = TextureManager::GetTexture("BottomInfoPanelShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(0), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(panel);

	auto font = FontManager::GetFont("Dominican");

	auto label = Interface::AddElement("PopulationLabel", new Text(font, textColor));
	label->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(0.0f, 0.0f)), nullptr);

	label->Enable();
	label->SetParent(panel);

	label = Interface::AddElement("BuildingLabel", new Text(font, textColor));
	label->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(0.0f, 50.0f)), nullptr);

	label->Enable();
	label->SetParent(panel);

	label = Interface::AddElement("DevelopmentLabel", new Text(font, textColor));
	label->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(0.0f, 100.0f)), nullptr);

	label->Enable();
	label->SetParent(panel);
}

void InterfaceBuilder::GenerateTopBar()
{
	auto texture = TextureManager::GetTexture("TopBar");
	auto sprite = new Sprite(texture, spriteShader);

	auto panel = Interface::AddElement(Elements::TOP_BAR, new TopBar());
	panel->Configure(Size(2560, 256), DrawOrder(1), new Transform(Position2(0.0f, -640.0f)), sprite, Opacity(1.0f));

	AddOpenAnimation(panel, -848.0f, -688.0f, InterfaceElementParameters::POSITION_Y);

	AddCloseAnimation(panel, -848.0f, -688.0f, InterfaceElementParameters::POSITION_Y);

	texture = TextureManager::GetTexture("TopBarShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(0), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(panel);
}

void InterfaceBuilder::GenerateTopPanel()
{
	auto texture = TextureManager::GetTexture("TopPanel");
	auto sprite = new Sprite(texture, spriteShader);

	auto panel = Interface::AddElement(Elements::TOP_PANEL, new TopPanel());
	panel->Configure(Size(256, 256), DrawOrder(2), new Transform(Position2(-1044.0f, -420.0f)), sprite, Opacity(1.0f));

	AddOpenAnimation(panel, -1800.0f, -1044.0f, InterfaceElementParameters::POSITION_X);

	AddCloseAnimation(panel, -1800.0f, -1044.0f, InterfaceElementParameters::POSITION_X);

	texture = TextureManager::GetTexture("TopPanelShadow");
	sprite = new Sprite(texture, shadowShader);

	auto shadow = Interface::AddElement("Shadow", new Element());
	shadow->Configure(Size(150, 150), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

	shadow->Enable();
	shadow->SetParent(panel);
}

void InterfaceBuilder::AddOpenAnimation(Element* element, float startHeight, float endHeight, InterfaceElementParameters parameter)
{
	auto animator = element->GetAnimator();
	if(animator == nullptr)
		return;

	auto animation = animator->AddAnimation(new Animation(0.5f), "Open");
	{
		auto property = element->AddAnimationProperty("Open", parameter);
		property->AddKey()->Initialize(0.0f, startHeight);
		property->AddKey()->Initialize(animation->GetLength(), endHeight);
	}
	{
		auto property = element->AddAnimationProperty("Open", InterfaceElementParameters::OPACITY);
		property->AddKey()->Initialize(0.0f, 0.0f);
		property->AddKey()->Initialize(animation->GetLength(), 1.0f);
	}
}

void InterfaceBuilder::AddCloseAnimation(Element* element, float startHeight, float endHeight, InterfaceElementParameters parameter)
{
	auto animator = element->GetAnimator();
	if(animator == nullptr)
		return;

	auto animation = animator->AddAnimation(new Animation(0.5f), "Close");
	{
		auto property = element->AddAnimationProperty("Close", parameter);
		property->AddKey()->Initialize(0.0f, endHeight);
		property->AddKey()->Initialize(animation->GetLength(), startHeight);
	}
	{
		auto property = element->AddAnimationProperty("Close", InterfaceElementParameters::OPACITY);
		property->AddKey()->Initialize(0.0f, 1.0f);
		property->AddKey()->Initialize(animation->GetLength(), 0.0f);
	}
}

