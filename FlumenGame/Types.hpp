#pragma once

#include "FlumenEngine/Core/Types.hpp"
#include "FlumenCore/Conventions.hpp"

enum class WorldSizeOptions
{
	TINY, SMALL, MEDIUM, LARGE, IMMENSE, COUNT
};

enum class ElementShapes
{
	SQUARE, ROUND
};

enum class ElementSizes
{
	SMALL, MEDIUM, LARGE
};

enum class ReliefTypes
{
	OCEAN, LAND
};

enum class WorldPreviewModes
{
	RELIEF, BIOME, POLITY, COUNT
};

enum class States
{
	NONE, INTRO, PREGAME, WORLD, BATTLE
};

enum class Scenes
{
	PREGAME, WORLD, BATTLE
};

enum class WorldObjects
{
	TILE, SETTLEMENT, PARTY, NONE
};

class FrameBuffers
{
public:
	static Word const DEFAULT;
	static Word const SHADOW_MAP;
	static Word const STENCIL;
};

class TerrainModelBuffers
{
public:
	static Word const INDEX_LINKS;
	static Word const POSITION_INPUT;
	static Word const POSITION_OUTPUT;
	static Word const NORMAL_OUTPUT;
	static Word const TANGENT_OUTPUT;
	static Word const INDICES;
	static int const COUNT;
};

class TerrainModelTextures
{
public:
	static Word const BASE_HEIGHT;
	static Word const DETAIL_HEIGHT;
	static Word const SHADOW_MAP;
	static Word const ROAD_STENCIL;
	static Word const ROAD_DETAIL;
	static Word const STEPPE_DIFFUSE;
	static int const COUNT;
};

class SettlementModelBuffers
{
public:
	static Word const VERTEX_POSITIONS;
	static Word const VERTEX_NORMALS;
	static Word const INDICES;
	static Word const TEXTURE_INDICES;
	static Word const BUILDING_DATAS;
	static Word const BUILDING_INDICES;
	static Word const CONNECTION_INDICES;
	static Word const LINK_DATAS;
	static int const COUNT;
};

class SettlementModelTextures
{
public:
	static Word const SHADOW_MAP;
	static Word const ROAD_ALPHA;
	static int const COUNT;
};

class Elements
{
public:
	static Word const MAIN_MENU;
	static Word const NEW_GAME_MENU;
	static Word const NEW_WORLD_MENU;
	static Word const WORLD_PREVIEW_PANEL;
	static Word const BOTTOM_INFO_PANEL;
	static Word const TOP_BAR;
	static Word const TOP_PANEL;
};

class Cameras
{
public:
	static Word const PREGAME;
	static Word const WORLD;
};
