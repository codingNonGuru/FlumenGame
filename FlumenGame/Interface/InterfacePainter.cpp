#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>

#include "InterfacePainter.hpp"

#include "FlumenEngine/Render/Image.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Utility/Perlin.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Utility/Kernel.hpp"
#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenEngine/Utility/Palette.hpp"
#include "FlumenEngine/Render/StencilManager.hpp"

Map <Image> InterfacePainter::images_ = Map <Image> (16);

Map <DataBuffer> InterfacePainter::buffers_ = Map <DataBuffer> (16);

Map <Array <ElementTextureSet>, InterfacePainter::ElementTextureClass> InterfacePainter::textureSets_ = Map <Array <ElementTextureSet>, ElementTextureClass> (16);

Palette basePalette = Palette(16);

Palette lightPalette = Palette(16);

Shader * InterfacePainter::shader_ = nullptr;

Shader * paperGenerationShader = nullptr;

Shader * cropShader = nullptr;

Color baseColor = Color(0.95f, 0.81f, 0.72f);

struct KernelBuffer
{
	Kernel* Kernel_;

	DataBuffer* Buffer_;

	KernelBuffer() {}

	KernelBuffer(Kernel* kernel, DataBuffer* buffer) {Kernel_ = kernel; Buffer_ = buffer;}
};

Map <KernelBuffer> kernelBuffers = Map <KernelBuffer> (8);

KernelBuffer* currentKernel = nullptr;

void InterfacePainter::Initialize()
{
	basePalette.Add(baseColor, Range(0.0f, 0.0f), Range(0.0f, 0.5f), Weight(1.5f));
	basePalette.Add(Color(1.0f, 0.0f, 0.0f), Range(-0.5f, -0.0f), Range(0.3f, 1.0f), Weight(1.0f));
	basePalette.Add(Color(1.0f, 0.3f, 0.0f), Range(-0.6f, -0.0f), Range(0.0f, 1.0f), Weight(0.5f));

	Size canvasSize(2048, 2048);

	*images_.Add("diffuse") = Image(canvasSize, ImageFormats::RGBA);
	*images_.Add("cache") = Image(canvasSize, ImageFormats::RGBA);
	*images_.Add("edgeBlur") = Image(canvasSize, ImageFormats::ALPHA);
	*images_.Add("edgeBlurSwap") = Image(canvasSize, ImageFormats::ALPHA);

	SetupKernels();

	GenerateStencils();

	paperGenerationShader = ShaderManager::GetShaderMap().Get("GeneratePaper");

	cropShader = ShaderManager::GetShaderMap().Get("Crop");
}

void InterfacePainter::SetupKernels()
{
	auto kernel = new Kernel(10);
	kernel->Initialize(KernelTypes::GAUSS, 50.0f);
	auto weights = kernel->GetWeights();

	auto buffer = buffers_.Add("tinyKernel");
	if(buffer)
	{
		*buffer = DataBuffer(weights.GetMemorySize(), weights.GetStart());
	}

	*kernelBuffers.Add("tiny") = KernelBuffer(kernel, buffer);

	kernel = new Kernel(20);
	kernel->Initialize(KernelTypes::GAUSS, 200.0f);
	weights = kernel->GetWeights();

	buffer = buffers_.Add("smallKernel");
	if(buffer)
	{
		*buffer = DataBuffer(weights.GetMemorySize(), weights.GetStart());
	}

	*kernelBuffers.Add("small") = KernelBuffer(kernel, buffer);

	kernel = new Kernel(40);
	kernel->Initialize(KernelTypes::GAUSS, 800.0f);
	weights = kernel->GetWeights();

	buffer = buffers_.Add("mediumKernel");
	if(buffer)
	{
		*buffer = DataBuffer(weights.GetMemorySize(), weights.GetStart());
	}

	*kernelBuffers.Add("medium") = KernelBuffer(kernel, buffer);

	kernel = new Kernel(100);
	kernel->Initialize(KernelTypes::GAUSS, 5000.0f);
	weights = kernel->GetWeights();

	buffer = buffers_.Add("largeKernel");
	if(buffer)
	{
		*buffer = DataBuffer(weights.GetMemorySize(), weights.GetStart());
	}

	*kernelBuffers.Add("large") = KernelBuffer(kernel, buffer);
}

DataBuffer* perlinBuffer = nullptr;

void InterfacePainter::GenerateStencils()
{
	Size sourceSize(2048, 2048);

	perlinBuffer = Perlin::Generate(sourceSize, FocusIndex(0.5f), 0.51f, 16.0f);

	Size stencilSize(512, 512);

	Length stencilCount = 16;
	unsigned int x = 0, y = 0;
	for(Index index = 0; index < stencilCount; ++index)
	{
		glm::uvec2 offset(x * stencilSize.x, y * stencilSize.y);

		auto stencil = new Image(stencilSize, perlinBuffer, sourceSize, offset);
		StencilManager::Add(stencil, "Paper", index);

		x++;
		if(x == sourceSize.x / stencilSize.x)
		{
			x = 0;
			y++;
		}
	}
}

void InterfacePainter::GeneratePaper(Size size, ElementShapes shape)
{
	Perlin::Generate(size, FocusIndex(0.2f), ContrastThreshold(0.5f), ContrastStrength(4.0f));

	SetupPaperGenerator(size);

	auto image = images_.Get("diffuse");
	image->Setup(size, baseColor);

	currentKernel = kernelBuffers.Get("tiny");

	Blur(size);

	ConvertBlurToAlpha(size);
}

void InterfacePainter::GenerateShadow(Size size, Texture*& texture)
{
	SetupPaperGenerator(size);

	currentKernel = kernelBuffers.Get("small");

	Blur(size);

	Grid <float> shadowMap(size.x, size.y);
	buffers_.Get("edgeBlurSwap")->Download(&shadowMap);

	shadowMap *= 0.7f;

	texture = new Texture(size, TextureFormats::ONE_FLOAT, &shadowMap);
}

void InterfacePainter::HighlightEdges(Size size, const char* kernelName)
{
	Perlin::Generate(size, FocusIndex(0.9f), 0.49f, 8.0f);

	SetupPaperGenerator(size);

	currentKernel = kernelBuffers.Get(kernelName);

	Blur(size);

	ConvertBlurToColor(paperGenerationShader, size);
}

void InterfacePainter::SetupPaperGenerator(Size size)
{
	buffers_.Get("diffuse")->Bind(0);
	perlinBuffer->Bind(1);
	buffers_.Get("edgeBlur")->Bind(2);
	buffers_.Get("edgeBlurSwap")->Bind(3);

	paperGenerationShader->Bind();

	paperGenerationShader->SetConstant(size, "size");
}

void InterfacePainter::Crop(Size size, Size sourceSize, Size offset, ElementShapes shape)
{
	cropShader->Bind();

	buffers_.Get("diffuse")->Bind(0);
	buffers_.Get("cache")->Bind(1);

	cropShader->SetConstant(size, "destinationSize");
	cropShader->SetConstant(sourceSize, "sourceSize");
	cropShader->SetConstant(offset, "offset");
	cropShader->SetConstant((int)shape, "shape");

	cropShader->DispatchCompute(size / 4);

	cropShader->Unbind();
}

void InterfacePainter::Blur(Size size)
{
	currentKernel->Buffer_->Bind(4);

	SetStage(Stages::BLUR_HORIZONTALLY);

	int filterSize = currentKernel->Kernel_->GetSide();
	paperGenerationShader->SetConstant(filterSize, "filterSize");

	paperGenerationShader->DispatchCompute(size / 16);

	SetStage(Stages::BLUR_VERTICALLY);

	filterSize = currentKernel->Kernel_->GetSide();
	paperGenerationShader->SetConstant(filterSize, "filterSize");

	paperGenerationShader->DispatchCompute(size / 16);
}

void InterfacePainter::ConvertBlurToAlpha(Size size)
{
	SetStage(Stages::CONVERT_BLUR_TO_ALPHA);

	paperGenerationShader->DispatchCompute(size / 16);
}

void InterfacePainter::ConvertBlurToColor(Shader* shader, Size size)
{
	SetStage(Stages::CONVERT_BLUR_TO_COLOR);

	shader->DispatchCompute(size / 16);
}

void InterfacePainter::ApplyBrushes(Size size)
{
	auto diffuseImage = images_.Get("diffuse");

	Length passCount = (size.x * size.y) / 800;
	for(Index pass = 0; pass < passCount; ++pass)
	{
		Index stencilIndex = utility::GetRandom(0, 15);
		auto stencil = StencilManager::Get("Paper", stencilIndex);

		auto stencilSize = stencil->GetSize();

		float alphaModifier = exp(-(float)pass / 700.0f);
		Float alpha = utility::GetRandom(0.2f, 0.3f) * alphaModifier + 0.02f;

		Size offset = Size(utility::GetRandom(0, size.x) - stencilSize.x / 2, utility::GetRandom(0, size.y) - stencilSize.y / 2);

		Color color = basePalette.GetColor();

		stencil->Apply(diffuseImage, alpha, color, offset);
	}
}

void InterfacePainter::SetStage(Stages stage)
{
	paperGenerationShader->SetConstant((int)stage, "stage");
}

void InterfacePainter::GenerateTextures(ElementShapes shape, Size size, Texture*& baseTexture)
{
	GeneratePaper(size, shape);

	ApplyBrushes(size);

	//HighlightEdges(size, size / 16, "large");

	HighlightEdges(size, "medium");

	//HighlightEdges(size, size / 16, "tiny");

	Grid <Color> diffuseMap(size.x, size.y);
	buffers_.Get("diffuse")->Download(&diffuseMap);

	baseTexture = new Texture(size, TextureFormats::FOUR_FLOAT, &diffuseMap);
}

void InterfacePainter::GeneratePaperBase(Size size)
{
	Size computeSize(size.x / 16, size.y / 16);

	SetupPaperGenerator(size);

	//Clear(paperGenerationShader, computeSize, ElementShapes::SQUARE);

	ApplyBrushes(size);

	buffers_.Get("diffuse")->Copy(buffers_.Get("cache"));
}

Texture* InterfacePainter::GenerateBaseTexture(Size size, Size sourceSize, Size offset, ElementShapes shape)
{
	Crop(size, sourceSize, offset, shape);

	Perlin::Generate(size, FocusIndex(0.2f), ContrastThreshold(0.5f), ContrastStrength(4.0f));

	SetupPaperGenerator(size);

	currentKernel = kernelBuffers.Get("tiny");

	Blur(size);

	ConvertBlurToAlpha(size);

	HighlightEdges(size, "medium");

	Grid <Color> diffuseMap(size.x, size.y);
	buffers_.Get("diffuse")->Download(&diffuseMap);

	return new Texture(size, TextureFormats::FOUR_FLOAT, &diffuseMap);
}

ElementTextureSet* InterfacePainter::GetTextureSet(ElementShapes shape, ElementSizes size, Index index)
{
	auto textureSetArray = textureSets_.Get(ElementTextureClass(shape, size));
	if(textureSetArray == nullptr)
		return nullptr;

	return textureSetArray->Get(index);
}

void InterfacePainter::PaintInterface()
{
	Initialize();

	Texture* baseTexture = nullptr;
	Texture* shadowTexture = nullptr;

	Size size = Size(768, 1024);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "MainMenu");
	TextureManager::AddTexture(shadowTexture, "MainMenuShadow");

	size = Size(768, 1024);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "NewGameMenu");
	TextureManager::AddTexture(shadowTexture, "NewGameMenuShadow");

	size = Size(768, 1024);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "NewWorldMenu");
	TextureManager::AddTexture(shadowTexture, "NewWorldMenuShadow");

	size = Size(768, 768);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "WorldPreviewPanel");
	TextureManager::AddTexture(shadowTexture, "WorldPreviewPanelShadow");

	size = Size(512, 640);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "BottomInfoPanel");
	TextureManager::AddTexture(shadowTexture, "BottomInfoPanelShadow");

	GeneratePaperBase(Size(1024, 1024));

	auto textureSetArray = textureSets_.Add(ElementTextureClass(ElementShapes::ROUND, ElementSizes::SMALL));
	textureSetArray->Initialize(4);

	for(int index = 0; index < textureSetArray->GetCapacity(); ++index)
	{
		size = Size(256, 256);
		baseTexture = GenerateBaseTexture(size, Size(1024, 1024), Size(0, (index % 4) * size.y), ElementShapes::ROUND);
		GenerateShadow(size, shadowTexture);
		*textureSetArray->Allocate() = ElementTextureSet(baseTexture, shadowTexture);
	}

	textureSetArray = textureSets_.Add(ElementTextureClass(ElementShapes::SQUARE, ElementSizes::SMALL));
	textureSetArray->Initialize(8);

	for(int index = 0; index < textureSetArray->GetCapacity(); ++index)
	{
		size = Size(512, 256);
		baseTexture = GenerateBaseTexture(size, Size(1024, 1024), Size((index / 2) * size.x, (index % 4) * size.y), ElementShapes::SQUARE);
		GenerateShadow(size, shadowTexture);
		*textureSetArray->Allocate() = ElementTextureSet(baseTexture, shadowTexture);
	}

	size = Size(2688, 256);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "TopBar");
	TextureManager::AddTexture(shadowTexture, "TopBarShadow");

	size = Size(512, 640);
	GenerateTextures(ElementShapes::SQUARE, size, baseTexture);
	GenerateShadow(size, shadowTexture);
	TextureManager::AddTexture(baseTexture, "TopPanel");
	TextureManager::AddTexture(shadowTexture, "TopPanelShadow");
}
