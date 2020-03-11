#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/FrameBuffer.hpp"
#include "FlumenEngine/Render/BufferManager.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Light.hpp"
#include "RenderBuilder.hpp"

#include "FlumenGame/TerrainModel.hpp"
#include "FlumenGame/Generators/ReliefGenerator.hpp"
#include "FlumenGame/Types.hpp"
#include "FlumenGame/Scenes/WorldScene.hpp"
#include "FlumenGame/SettlementRenderer.hpp"

enum class Shaders {DISPLACE, RENDER};

TerrainModel::TerrainModel()
{
	auto world = WorldScene::GetWorld();
	ReliefGenerator::GenerateModel(*world);

	DataBuffer* buffer = nullptr;
	buffers_.Initialize(TerrainModelBuffers::COUNT);

	buffer = ReliefGenerator::GetModelBuffer(TerrainModelBuffers::POSITION_INPUT);
	*buffers_.Add(TerrainModelBuffers::POSITION_INPUT) = buffer;
	auto bufferSize = buffer->GetMemorySize();

	buffer = ReliefGenerator::GetModelBuffer(TerrainModelBuffers::INDEX_LINKS);
	*buffers_.Add(TerrainModelBuffers::INDEX_LINKS) = buffer;

	buffer = ReliefGenerator::GetModelBuffer(TerrainModelBuffers::INDICES);
	*buffers_.Add(TerrainModelBuffers::INDICES) = buffer;

	buffer = new DataBuffer(bufferSize);
	*buffers_.Add(TerrainModelBuffers::POSITION_OUTPUT) = buffer;

	buffer = new DataBuffer(bufferSize);
	*buffers_.Add(TerrainModelBuffers::TANGENT_OUTPUT) = buffer;

	buffer = new DataBuffer(bufferSize);
	*buffers_.Add(TerrainModelBuffers::NORMAL_OUTPUT) = buffer;

	shaders_.Initialize(2);

	auto shader = shaders_.Add(Shaders::DISPLACE);
	*shader = ShaderManager::GetShader("Displace");

	shader = shaders_.Add(Shaders::RENDER);
	*shader = ShaderManager::GetShader("Land");

	textures_.Initialize(TerrainModelTextures::COUNT);

	auto texture = ReliefGenerator::GetModelTexture(TerrainModelTextures::BASE_HEIGHT);
	*textures_.Add(TerrainModelTextures::BASE_HEIGHT) = texture;

	texture = ReliefGenerator::GetModelTexture(TerrainModelTextures::DETAIL_HEIGHT);
	*textures_.Add(TerrainModelTextures::DETAIL_HEIGHT) = texture;

	texture = ReliefGenerator::GetModelTexture(TerrainModelTextures::ROAD_DETAIL);
	*textures_.Add(TerrainModelTextures::ROAD_DETAIL) = texture;

	auto shadowFrameBuffer = BufferManager::GetFrameBuffer(FrameBuffers::SHADOW_MAP);
	texture = shadowFrameBuffer->GetDepthTexture();
	*textures_.Add(TerrainModelTextures::SHADOW_MAP) = texture;

	auto stencilFrameBuffer = BufferManager::GetFrameBuffer(FrameBuffers::STENCIL);
	texture = stencilFrameBuffer->GetColorTexture();
	*textures_.Add(TerrainModelTextures::ROAD_STENCIL) = texture;
}

void TerrainModel::Initialize()
{

}

#include "FlumenCore/Time.hpp"

void TerrainModel::Render(Camera* camera, Light* light)
{
	Time::StartClock();

	auto displaceShader = *shaders_.Get(Shaders::DISPLACE);
	if(displaceShader == nullptr)
		return;

	displaceShader->Bind();

	DataBuffer* buffer = nullptr;

	buffer = *buffers_.Get(TerrainModelBuffers::INDEX_LINKS);
	buffer->Bind(0);

	buffer = *buffers_.Get(TerrainModelBuffers::POSITION_INPUT);
	buffer->Bind(1);
	Length vertexCount = buffer->GetSize();

	buffer = *buffers_.Get(TerrainModelBuffers::POSITION_OUTPUT);
	buffer->Bind(2);

	buffer = *buffers_.Get(TerrainModelBuffers::NORMAL_OUTPUT);
	buffer->Bind(3);

	buffer = *buffers_.Get(TerrainModelBuffers::TANGENT_OUTPUT);
	buffer->Bind(4);

	Texture* texture = nullptr;

	texture = *textures_.Get(TerrainModelTextures::BASE_HEIGHT);
	displaceShader->BindTexture(texture, "reliefHeightMap");

	texture = *textures_.Get(TerrainModelTextures::DETAIL_HEIGHT);
	displaceShader->BindTexture(texture, "reliefDetailMap");

	texture = *textures_.Get(TerrainModelTextures::ROAD_STENCIL);
	displaceShader->BindTexture(texture, "roadStencil");

	auto viewDistance = camera->GetViewDistance();
	displaceShader->SetConstant(viewDistance, "size");

	auto center = camera->GetPosition();
	displaceShader->SetConstant(Float2(center.x, center.y), "center");
	displaceShader->SetConstant(vertexCount, "vertexCount");

	auto azimuth = camera->GetAzimuth();
	displaceShader->SetConstant((float)cos(azimuth), "c");
	displaceShader->SetConstant((float)sin(azimuth), "s");

	displaceShader->SetConstant(ReliefGenerator::DETAIL_STRENGTH, "strength");

	displaceShader->SetConstant(ReliefGenerator::DETAIL_TILE_COUNT, "detailTileCount");

	auto stencilData = SettlementRenderer::GetStencilData();
	displaceShader->SetConstant(stencilData.Offset_, "stencilPosition");
	displaceShader->SetConstant(stencilData.Scale_, "stencilScale");

	Length workSize = vertexCount / 32 + 1;

	displaceShader->SetConstant(0, "mode");
	displaceShader->DispatchCompute(workSize);

	displaceShader->SetConstant(1, "mode");
	displaceShader->DispatchCompute(workSize);

	displaceShader->Unbind();

	auto renderShader = *shaders_.Get(Shaders::RENDER);
	if(renderShader == nullptr)
		return;

	renderShader->Bind();

	buffer = *buffers_.Get(TerrainModelBuffers::POSITION_OUTPUT);
	buffer->Bind(0);

	buffer = *buffers_.Get(TerrainModelBuffers::INDICES);
	buffer->Bind(1);
	Length indexCount = buffer->GetSize();

	buffer = *buffers_.Get(TerrainModelBuffers::NORMAL_OUTPUT);
	buffer->Bind(2);

	buffer = *buffers_.Get(TerrainModelBuffers::TANGENT_OUTPUT);
	buffer->Bind(3);

	texture = *textures_.Get(TerrainModelTextures::SHADOW_MAP);
	renderShader->BindTexture(texture, "shadowMap");

	texture = *textures_.Get(TerrainModelTextures::ROAD_STENCIL);
	renderShader->BindTexture(texture, "roadStencil");

	texture = *textures_.Get(TerrainModelTextures::ROAD_DETAIL);
	renderShader->BindTexture(texture, "roadDetail");

	renderShader->SetConstant(camera->GetMatrix(), "projMatrix");
	auto depthMatrix = light->GetShadowMatrix(camera->GetViewDistance() * RenderBuilder::SHADOW_MAP_SIZE_MODIFIER, camera->GetTarget());
	renderShader->SetConstant(depthMatrix, "depthMatrix");
	renderShader->SetConstant(light->GetDirection(), "lightDirection");
	renderShader->SetConstant(camera->GetPosition(), "cameraPos");
	renderShader->SetConstant(ReliefGenerator::SEA_LEVEL, "seaLevel");

	stencilData = SettlementRenderer::GetStencilData();
	renderShader->SetConstant(stencilData.Offset_, "stencilOffset");
	renderShader->SetConstant(stencilData.Scale_, "stencilScale");

	glDrawArrays(GL_TRIANGLES, 0, indexCount);

	DEBUG_OPENGL

	renderShader->Unbind();

	//std::cout<<"------------> CLOCK: "<<Time::GetClock()<<"\n";
}
