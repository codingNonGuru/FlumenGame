#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Light.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Interface.hpp"

#include "FlumenGame/Scenes/WorldScene.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Game.hpp"
#include "FlumenGame/TerrainModel.hpp"
#include "FlumenGame/Types.hpp"
#include "FlumenGame/SettlementRenderer.hpp"
#include "FlumenGame/Generators/CultureModelBuilder.hpp"

WorldScene* WorldScene::instance_ = nullptr;

WorldScene::WorldScene()
{
	if(instance_ != nullptr)
		return;

	instance_ = this;

	Game::OnStartGame_.Add(this, &WorldScene::HandleStartGame);

	world_ = nullptr;

	worldModel_ = nullptr;

	camera_ = RenderManager::GetCamera(Cameras::WORLD);

	light_ = new Light(Float3(1.0f, 1.0f, 1.0f));
}

void WorldScene::Initialize(File* file)
{
}

void WorldScene::Initialize(const WorldParameterSet& parameterSet)
{
	world_ = new World(parameterSet);
}

const float spinModifier = 0.005f;

const float pushModifier = 0.005f;

const float zoomModifier = 0.005f;

void WorldScene::Update()
{
	if(InputHandler::WasPressed(SDL_SCANCODE_SPACE))
	{
		if(world_->IsUpdating())
		{
			world_->Stop();
		}
		else
		{
			world_->Start();
		}
	}

	if(InputHandler::IsPressed(SDL_SCANCODE_A))
	{
		camera_->Spin(-spinModifier);
	}
	else if(InputHandler::IsPressed(SDL_SCANCODE_D))
	{
		camera_->Spin(spinModifier);
	}

	auto viewDistance = camera_->GetViewDistance();
	if(InputHandler::IsPressed(SDL_SCANCODE_W))
	{
		camera_->PushForward(-viewDistance * pushModifier);
	}
	else if(InputHandler::IsPressed(SDL_SCANCODE_S))
	{
		camera_->PushForward(viewDistance * pushModifier);
	}

	auto & mouse = InputHandler::GetMouse();
	if(mouse.ScrollUp_)
	{
		camera_->Zoom(viewDistance * zoomModifier);
	}
	else if(mouse.ScrollDown_)
	{
		camera_->Zoom(-viewDistance * zoomModifier);
	}

	ProcessSelection();

	world_->Update();
}

WorldObject currentSelection = WorldObject::DEFAULT;

WorldObject hoverTarget = WorldObject::DEFAULT;

void WorldScene::ProcessSelection()
{
	auto screen = Engine::GetScreen();

	auto mouse = InputHandler::GetMouse();
	auto mousePosition = InputHandler::GetMousePosition(false);
	float mouseX = ((mousePosition.x / screen->getWidthFloating()) * 2.0f) - 1.0f;
	float mouseY = 1.0f - ((mousePosition.y / screen->getHeightFloating()) * 2.0f);

	Float4 tempRay = glm::inverse(camera_->GetMatrix()) * Float4(mouseX, mouseY, 1.0f, 1.0f);
	tempRay /= tempRay.w;
	Float3 ray(tempRay.x, tempRay.y, tempRay.z);

	hoverTarget = world_->ProcessSelection(camera_, ray, Float2(mouseX, mouseY));

	if(mouse.CurrentLeft_)
	{
		if(hoverTarget.Object_ != nullptr && hoverTarget.Object_ != currentSelection.Object_)
		{
			currentSelection = hoverTarget;
			bottomInfoPanel_->Open();
		}

		if(hoverTarget.Object_ == nullptr && currentSelection.Object_ != nullptr)
		{
			currentSelection = hoverTarget;
			bottomInfoPanel_->Close();
		}
	}

	/*if(currentSelection.Object_ != nullptr && hoverTarget.Object_ != nullptr && currentSelection.Object_ != hoverTarget.Object_)
	{
		auto thisSet = (Settlement*)currentSelection.Object_;
		auto otherSet = (Settlement*)hoverTarget.Object_;

		auto direction = otherSet->GetPosition() - thisSet->GetPosition();
		std::cout<<glm::length(direction)<<"          ";
		direction = glm::normalize(direction);

		auto network = thisSet->GetLinkNetwork();
		for(auto link = network.GetFirst(); link != network.GetLast(); ++link)
		{
			auto otherDirection = link->Other_->GetPosition() - thisSet->GetPosition();
			otherDirection = glm::normalize(otherDirection);

			auto angle = glm::dot(direction, otherDirection);
			angle = acos(angle);

			std::cout<<angle<<" ";
		}
		std::cout<<"\n";
	}*/
}

void WorldScene::Render()
{
	RenderManager::EnableDepthTesting();

	SettlementRenderer::ProjectShadows(camera_, light_);

	SettlementRenderer::UpdateStencils(camera_);

	if(worldModel_ != nullptr)
	{
		worldModel_->Render(camera_, light_);
	}

	SettlementRenderer::Update(camera_, light_);
}

World* WorldScene::GetWorld()
{
	if(instance_ == nullptr)
		return nullptr;

	return instance_->world_;
}

WorldObject* WorldScene::GetSelectedObject()
{
	return &currentSelection;
}

WorldObject* WorldScene::GetHoveredObject()
{
	return &hoverTarget;
}

void WorldScene::HandleStartGame()
{
	worldModel_ = new TerrainModel();

	bottomInfoPanel_ = Interface::GetElement(Elements::BOTTOM_INFO_PANEL);

	CultureModelBuilder::Generate(*world_);
}
