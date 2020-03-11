#include "FlumenGame/Types.hpp"

Word const FrameBuffers::DEFAULT = Word("Default");
Word const FrameBuffers::SHADOW_MAP = Word("ShadowMap");
Word const FrameBuffers::STENCIL = Word("Stencil");

Word const TerrainModelBuffers::INDEX_LINKS = Word("IndexLinks");
Word const TerrainModelBuffers::POSITION_INPUT = Word("PositionInput");
Word const TerrainModelBuffers::POSITION_OUTPUT = Word("PositionOutput");
Word const TerrainModelBuffers::NORMAL_OUTPUT = Word("NormalOutput");
Word const TerrainModelBuffers::TANGENT_OUTPUT = Word("TangentOutput");
Word const TerrainModelBuffers::INDICES = Word("Indices");
int const TerrainModelBuffers::COUNT = 6;

Word const TerrainModelTextures::BASE_HEIGHT = Word("BaseHeight");
Word const TerrainModelTextures::DETAIL_HEIGHT = Word("DetailHeight");
Word const TerrainModelTextures::SHADOW_MAP = Word("ShadowMap");
Word const TerrainModelTextures::ROAD_STENCIL = Word("RoadStencil");
Word const TerrainModelTextures::ROAD_DETAIL = Word("RoadDetail");
Word const TerrainModelTextures::STEPPE_DIFFUSE = Word("SteppeDiffuse");
int const TerrainModelTextures::COUNT = 6;

Word const SettlementModelBuffers::VERTEX_POSITIONS = Word("VertexPositions");
Word const SettlementModelBuffers::VERTEX_NORMALS = Word("VertexNormals");
Word const SettlementModelBuffers::INDICES = Word("Indices");
Word const SettlementModelBuffers::BUILDING_DATAS = Word("BuildingDatas");
Word const SettlementModelBuffers::BUILDING_INDICES = Word("BuildingIndices");
Word const SettlementModelBuffers::TEXTURE_INDICES = Word("TextureIndices");
Word const SettlementModelBuffers::CONNECTION_INDICES = Word("ConnectionIndices");
Word const SettlementModelBuffers::LINK_DATAS = Word("LinkDatas");
int const SettlementModelBuffers::COUNT = 8;

Word const SettlementModelTextures::SHADOW_MAP = Word("ShadowMap");;
Word const SettlementModelTextures::ROAD_ALPHA = Word("RoadAlpha");
int const SettlementModelTextures::COUNT = 2;

Word const Elements::MAIN_MENU = Word("MainMenu");
Word const Elements::NEW_GAME_MENU = Word("NewGameMenu");
Word const Elements::NEW_WORLD_MENU = Word("NewWorldMenu");
Word const Elements::WORLD_PREVIEW_PANEL = Word("WorldPreviewPanel");
Word const Elements::BOTTOM_INFO_PANEL = Word("BottomInfoPanel");
Word const Elements::TOP_BAR = Word("TopBar");
Word const Elements::TOP_PANEL = Word("TopPanel");

Word const Cameras::PREGAME = Word("Pregame");
Word const Cameras::WORLD = Word("World");
