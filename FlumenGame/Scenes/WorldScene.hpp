#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class World;
struct WorldParameterSet;
struct WorldObject;
class File;
class TerrainModel;
class SettlementModel;
class Element;

class WorldScene : public Scene
{
	static WorldScene* instance_;

	World* world_;

	TerrainModel* worldModel_;

	Element* bottomInfoPanel_;

	void HandleStartGame();

	void ProcessSelection();

public:
	WorldScene();

	void Initialize(File*);

	void Initialize(const WorldParameterSet&);

	void Update() override;

	void Render() override;

	static World* GetWorld();

	static WorldObject* GetSelectedObject();

	static WorldObject* GetHoveredObject();
};
