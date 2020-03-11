#include <glm/gtx/transform.hpp>

#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Mesh.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/BufferManager.hpp"
#include "FlumenEngine/Render/Light.hpp"
#include "FlumenEngine/Render/FrameBuffer.hpp"
#include "RenderBuilder.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"

#include "FlumenGame/SettlementRenderer.hpp"
#include "FlumenGame/Scenes/WorldScene.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Tile.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/Types.hpp"

SettlementRenderer* SettlementRenderer::instance_ = nullptr;

StencilData SettlementRenderer::stencilData_ = StencilData();

const int SettlementRenderer::BUILDING_RENDER_CAPACITY = 65536;

const int SettlementRenderer::CONNECTION_RENDER_CAPACITY = 64;

const float SettlementRenderer::ROAD_STENCIL_CAMERA_MODIFIER = 3.0f;

enum class Shaders {BUILDING, BUILDING_SHADOW, ROAD_STENCIL, BUILDING_PAVE, COUNT};

SettlementRenderer* SettlementRenderer::GetInstance()
{
	if(instance_ == nullptr)
		instance_ = new SettlementRenderer();

	return instance_;
}

SettlementRenderer::SettlementRenderer()
{
	isInitialized_ = false;

	shaders_.Initialize((int)Shaders::COUNT);

	auto shader = shaders_.Add(Shaders::BUILDING);
	*shader = ShaderManager::GetShader("Building");

	shader = shaders_.Add(Shaders::BUILDING_SHADOW);
	*shader = ShaderManager::GetShader("BuildingShadow");

	shader = shaders_.Add(Shaders::ROAD_STENCIL);
	*shader = ShaderManager::GetShader("Road");

	shader = shaders_.Add(Shaders::BUILDING_PAVE);
	*shader = ShaderManager::GetShader("BuildingPave");

	buffers_.Initialize(SettlementModelBuffers::COUNT);

	AssembleMesh();

	auto buffer = buffers_.Add(SettlementModelBuffers::BUILDING_INDICES);
	*buffer = new DataBuffer(sizeof(Index) * BUILDING_RENDER_CAPACITY, nullptr);

	buffer = buffers_.Add(SettlementModelBuffers::CONNECTION_INDICES);
	*buffer = new DataBuffer(sizeof(Index) * CONNECTION_RENDER_CAPACITY, nullptr);

	textures_.Initialize(SettlementModelTextures::COUNT);

	buildingIndices_.Initialize(BUILDING_RENDER_CAPACITY);

	connectionIndices_.Initialize(CONNECTION_RENDER_CAPACITY);
}

void SettlementRenderer::Initialize()
{
	if(isInitialized_)
		return;

	auto shadowFrameBuffer = BufferManager::GetFrameBuffer(FrameBuffers::SHADOW_MAP);
	auto texture = shadowFrameBuffer->GetDepthTexture();
	*textures_.Add(SettlementModelTextures::SHADOW_MAP) = texture;

	texture = TextureManager::GetTexture("RoadAlpha");
	*textures_.Add(SettlementModelTextures::ROAD_ALPHA) = texture;

	isInitialized_ = true;
}

void SettlementRenderer::AssembleMesh()
{
	meshes_.Initialize(7);

	*meshes_.Add(0) = MeshManager::GetMesh("Building0");
	*meshes_.Add(1) = MeshManager::GetMesh("Building1");
	*meshes_.Add(2) = MeshManager::GetMesh("Building5");
	*meshes_.Add(3) = MeshManager::GetMesh("Building2");
	*meshes_.Add(4) = MeshManager::GetMesh("Building6");
	*meshes_.Add(5) = MeshManager::GetMesh("Building3");
	*meshes_.Add(6) = MeshManager::GetMesh("Building4");

	Length positionCapacity = 0;
	Length normalCapacity = 0;
	Length indexCapacity = 0;
	Length textureIndexCapacity = 0;

	defaultMeshSize_ = 0;

	for(auto meshIterator = meshes_.GetStart(); meshIterator != meshes_.GetEnd(); ++meshIterator)
	{
		auto mesh = *meshIterator;

		auto & meshAttributes = mesh->GetAttributes();

		auto attributeData = meshAttributes.Get("position")->GetData();
		positionCapacity += attributeData->GetMemoryCapacity();

		attributeData = meshAttributes.Get("normal")->GetData();
		normalCapacity += attributeData->GetMemoryCapacity();

		attributeData = meshAttributes.Get("index")->GetData();
		indexCapacity += attributeData->GetMemoryCapacity();

		attributeData = meshAttributes.Get("textureIndex")->GetData();
		textureIndexCapacity += attributeData->GetMemoryCapacity();

		if(mesh->GetIndexCount() > defaultMeshSize_)
			defaultMeshSize_ = mesh->GetIndexCount();
	}

	auto positionData = new Byte[positionCapacity];
	auto normalData = new Byte[normalCapacity];
	auto indexData = new Byte[indexCapacity];
	auto textureIndexData = new Byte[textureIndexCapacity];

	Index positionIndex = 0;
	Index normalIndex = 0;
	Index indexIndex = 0;
	Index textureIndexIndex = 0;

	for(auto meshIterator = meshes_.GetStart(); meshIterator != meshes_.GetEnd(); ++meshIterator)
	{
		auto mesh = *meshIterator;

		auto & meshAttributes = mesh->GetAttributes();

		Index lastPositionIndex = positionIndex;

		auto attributeData = meshAttributes.Get("position")->GetData();
		CopyMemory(positionData + positionIndex, attributeData->GetData(), attributeData->GetMemoryCapacity());
		positionIndex += attributeData->GetMemoryCapacity();

		attributeData = meshAttributes.Get("normal")->GetData();
		CopyMemory(normalData + normalIndex, attributeData->GetData(), attributeData->GetMemoryCapacity());
		normalIndex += attributeData->GetMemoryCapacity();

		attributeData = meshAttributes.Get("index")->GetData();
		CopyMemory(indexData + indexIndex, attributeData->GetData(), attributeData->GetMemoryCapacity());

		for(Index* index = (Index*)indexData + indexIndex / 4; index != (Index*)indexData + indexCapacity / 4; index++)
		{
			*index = *index + lastPositionIndex / 12;
		}

		indexIndex += attributeData->GetMemoryCapacity();

		attributeData = meshAttributes.Get("textureIndex")->GetData();
		CopyMemory(textureIndexData + textureIndexIndex, attributeData->GetData(), attributeData->GetMemoryCapacity());
		textureIndexIndex += attributeData->GetMemoryCapacity();
	}

	*buffers_.Add(SettlementModelBuffers::VERTEX_POSITIONS) = new DataBuffer(positionCapacity, positionData);

	*buffers_.Add(SettlementModelBuffers::VERTEX_NORMALS) = new DataBuffer(normalCapacity, normalData);

	*buffers_.Add(SettlementModelBuffers::INDICES) = new DataBuffer(indexCapacity, indexData);

	*buffers_.Add(SettlementModelBuffers::TEXTURE_INDICES) = new DataBuffer(textureIndexCapacity, textureIndexData);
}

void SettlementRenderer::ProcessData(Camera* camera)
{
	auto world = WorldScene::GetWorld();

	buildingIndices_.Reset();

	auto centralPosition = camera->GetTarget();
	auto & tiles = world->GetTiles();
	int range = 30;
	for(int x = centralPosition.x - range; x < centralPosition.x + range; ++x)
	{
		for(int y = centralPosition.y - range; y < centralPosition.y + range; ++y)
		{
			auto tile = tiles.Get(x, y);
			auto settlement = tile->GetSettlement();
			if(settlement == nullptr)
				continue;

			auto renderData = settlement->GetRenderData();
			for(auto index = renderData->BuildingIndex_; index < renderData->BuildingIndex_ + renderData->BuildingCount_; ++index)
			{
				auto indexPointer = buildingIndices_.Allocate();
				if(indexPointer == nullptr)
					continue;

				*indexPointer = index;
			}
		}
	}

	DataBuffer* buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_INDICES);
	buffer->UploadData(buildingIndices_.GetStart(), buildingIndices_.GetMemorySize());

	connectionIndices_.Reset();

	auto selection = WorldScene::GetSelectedObject();
	if(selection->Object_ == nullptr)
		return;

	auto settlement = (Settlement*)selection->Object_;
	auto linkNetwork = settlement->GetLinkNetwork();

	for(auto link = linkNetwork.GetFirst(); link != linkNetwork.GetLast(); ++link)
	{
		*connectionIndices_.Allocate() = link->Other_->GetKey();
	}

	buffer = *buffers_.Get(SettlementModelBuffers::CONNECTION_INDICES);
	buffer->UploadData(connectionIndices_.GetStart(), connectionIndices_.GetMemorySize());
}

void SettlementRenderer::RenderStencils(Camera* camera)
{
	Initialize();

	BufferManager::BindFrameBuffer(FrameBuffers::STENCIL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto roadShader = *shaders_.Get(Shaders::ROAD_STENCIL);
	if(roadShader == nullptr)
		return;

	roadShader->Bind();

	auto buffer = *buffers_.Get(SettlementModelBuffers::LINK_DATAS);
	buffer->Bind(0);

	stencilData_.Scale_ = Float2(camera->GetViewDistance(), camera->GetViewDistance()) * ROAD_STENCIL_CAMERA_MODIFIER;

	auto target = camera->GetTarget();
	stencilData_.Offset_ = Float2(target.x, target.y) - stencilData_.Scale_ * 0.5f;

	Matrix projectionMatrix = glm::ortho<float> (0.0f, stencilData_.Scale_.x, stencilData_.Scale_.y, 0.0f, 0.1f, 10.0f);
	Matrix viewMatrix = glm::lookAt<float> (Float3(0.0f, 0.0f, 1.0f), Float3(0.0f), Float3(0.0f, 1.0f, 0.0f));
	Matrix finalMatrix = projectionMatrix * viewMatrix;

	roadShader->SetConstant(finalMatrix, "viewMatrix");

	roadShader->SetConstant(stencilData_.Offset_, "stencilOffset");

	roadShader->SetConstant(stencilData_.Scale_, "stencilScale");

	Texture* texture = *textures_.Get(SettlementModelTextures::ROAD_ALPHA);
	roadShader->BindTexture(texture, "roadAlpha");

	glDrawArrays(GL_TRIANGLES, 0, linkDatas_.GetSize() * 6);

	roadShader->Unbind();

	auto paveShader = *shaders_.Get(Shaders::BUILDING_PAVE);
	if(paveShader == nullptr)
		return;

	paveShader->Bind();

	buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_INDICES);
	buffer->Bind(0);

	buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_DATAS);
	buffer->Bind(1);

	paveShader->SetConstant(finalMatrix, "viewMatrix");

	paveShader->SetConstant(stencilData_.Offset_, "stencilOffset");

	paveShader->SetConstant(stencilData_.Scale_, "stencilScale");

	glDrawArrays(GL_TRIANGLES, 0, buildingIndices_.GetSize() * 6);

	paveShader->Unbind();

	BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);
}

void SettlementRenderer::RenderShadows(Camera* camera, Light* light)
{
	Initialize();

	BufferManager::BindFrameBuffer(FrameBuffers::SHADOW_MAP);

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	auto depthMatrix = light->GetShadowMatrix(camera->GetViewDistance() * RenderBuilder::SHADOW_MAP_SIZE_MODIFIER, camera->GetTarget());

	ProcessData(camera);

	auto buildingShader = *shaders_.Get(Shaders::BUILDING_SHADOW);
	if(buildingShader == nullptr)
		return;

	buildingShader->Bind();

	DataBuffer* buffer = nullptr;

	buffer = *buffers_.Get(SettlementModelBuffers::VERTEX_POSITIONS);
	buffer->Bind(0);

	buffer = *buffers_.Get(SettlementModelBuffers::INDICES);
	buffer->Bind(2);

	buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_INDICES);
	buffer->Bind(3);

	buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_DATAS);
	buffer->Bind(4);

	buildingShader->SetConstant(depthMatrix, "depthMatrix");
	buildingShader->SetConstant(defaultMeshSize_, "indexCount");

	glDrawArrays(GL_TRIANGLES, 0, buildingIndices_.GetSize() * defaultMeshSize_);

	buildingShader->Unbind();

	BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);
}

void SettlementRenderer::Render(Camera* camera, Light* light)
{
	Initialize();

	auto buildingShader = *shaders_.Get(Shaders::BUILDING);
	if(buildingShader == nullptr)
		return;

	buildingShader->Bind();

	DataBuffer* buffer = nullptr;

	buffer = *buffers_.Get(SettlementModelBuffers::VERTEX_POSITIONS);
	buffer->Bind(0);

	buffer = *buffers_.Get(SettlementModelBuffers::VERTEX_NORMALS);
	buffer->Bind(1);

	buffer = *buffers_.Get(SettlementModelBuffers::INDICES);
	buffer->Bind(2);

	buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_INDICES);
	buffer->Bind(3);

	buffer = *buffers_.Get(SettlementModelBuffers::BUILDING_DATAS);
	buffer->Bind(4);

	buffer = *buffers_.Get(SettlementModelBuffers::TEXTURE_INDICES);
	buffer->Bind(5);

	buffer = *buffers_.Get(SettlementModelBuffers::CONNECTION_INDICES);
	buffer->Bind(6);

	Texture* texture = *textures_.Get(SettlementModelTextures::SHADOW_MAP);
	buildingShader->BindTexture(texture, "shadowMap");

	buildingShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	auto depthMatrix = light->GetShadowMatrix(camera->GetViewDistance() * RenderBuilder::SHADOW_MAP_SIZE_MODIFIER, camera->GetTarget());
	buildingShader->SetConstant(depthMatrix, "depthMatrix");

	buildingShader->SetConstant(defaultMeshSize_, "indexCount");

	buildingShader->SetConstant(camera->GetPosition(), "cameraPosition");

	buildingShader->SetConstant(connectionIndices_.GetSize(), "connectionCount");

	glDrawArrays(GL_TRIANGLES, 0, buildingIndices_.GetSize() * defaultMeshSize_);

	buildingShader->Unbind();
}

void SettlementRenderer::Update(Camera* camera, Light* light)
{
	GetInstance()->Render(camera, light);
}

void SettlementRenderer::ProjectShadows(Camera* camera, Light* light)
{
	GetInstance()->RenderShadows(camera, light);
}

void SettlementRenderer::UpdateStencils(Camera* camera)
{
	GetInstance()->RenderStencils(camera);
}

Array <SettlementRenderData> & SettlementRenderer::GetSettlementDatas()
{
	return GetInstance()->settlementDatas_;
}

Array <BuildingRenderData> & SettlementRenderer::GetBuildingDatas()
{
	return GetInstance()->buildingDatas_;
}

Array <LinkRenderData> & SettlementRenderer::GetLinkDatas()
{
	return GetInstance()->linkDatas_;
}

Map <DataBuffer*> & SettlementRenderer::GetBuffers()
{
	return GetInstance()->buffers_;
}

StencilData SettlementRenderer::GetStencilData()
{
	return stencilData_;
}

Mesh* SettlementRenderer::GetMesh(Index index)
{
	auto mesh = GetInstance()->meshes_.Get(index);
	if(mesh == nullptr)
		return nullptr;

	return *mesh;
}
