#pragma once

#include "FlumenEngine/Render/Model.hpp"

struct BuildingRenderData
{
	Float3 Position_;

	Float Rotation_;

	Index MeshIndex_;

	Index SettlementIndex_;
};

struct SettlementRenderData
{
	Index BuildingIndex_;

	Index BuildingCount_;

	BuildingRenderData* Buildings_;
};

struct LinkRenderData
{
	Float2 Position_;

    Float Rotation_;

    Float Length_;

    Index TextureIndex_;

	LinkRenderData() {}

	LinkRenderData(Float2 Position, Float Rotation, Float Length, Index TextureIndex) : Position_(Position), Rotation_(Rotation), Length_(Length), TextureIndex_(TextureIndex) {}
};

struct StencilData
{
	Float2 Offset_;

	Float2 Scale_;

	StencilData() {}

	StencilData(Float2 Offset, Float2 Scale) : Offset_(Offset), Scale_(Scale) {}
};

class SettlementRenderer : public Model
{
	static SettlementRenderer* instance_;

	static StencilData stencilData_;

	Array <BuildingRenderData> buildingDatas_;

	Array <SettlementRenderData> settlementDatas_;

	Array <LinkRenderData> linkDatas_;

	Array <Index> buildingIndices_;

	Array <Index> connectionIndices_;

	Length defaultMeshSize_;

	static SettlementRenderer* GetInstance();

	SettlementRenderer();

	virtual void Render(Camera*, Light*);

	virtual void Initialize();

	void RenderShadows(Camera*, Light*);

	void RenderStencils(Camera*);

	void ProcessData(Camera*);

	void AssembleMesh();

public:
	static void Update(Camera*, Light*);

	static void ProjectShadows(Camera*, Light*);

	static void UpdateStencils(Camera*);

	static Array <SettlementRenderData> & GetSettlementDatas();

	static Array <BuildingRenderData> & GetBuildingDatas();

	static Array <LinkRenderData> & GetLinkDatas();

	static Map <DataBuffer*> & GetBuffers();

	static Mesh* GetMesh(Index);

	static StencilData GetStencilData();

	static const int BUILDING_RENDER_CAPACITY;

	static const int CONNECTION_RENDER_CAPACITY;

	static const float ROAD_STENCIL_CAMERA_MODIFIER;
};
