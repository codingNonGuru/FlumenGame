#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenGame/Types.hpp"

class Texture;
class Image;
class DataBuffer;
class Shader;

struct ElementTextureSet
{
	Texture* Base_;

	Texture* Shadow_;

	ElementTextureSet() {}

	ElementTextureSet(Texture* Base, Texture* Shadow) : Base_(Base), Shadow_(Shadow) {}
};

class InterfacePainter
{
	enum class Stages
	{
		CLEAR, BLUR_HORIZONTALLY, BLUR_VERTICALLY, CONVERT_BLUR_TO_ALPHA, CONVERT_BLUR_TO_COLOR
	};

	struct ElementTextureClass
	{
		ElementShapes Shape_;

		ElementSizes Size_;

		ElementTextureClass() {}

		ElementTextureClass(ElementShapes Shape, ElementSizes Size) : Shape_(Shape), Size_(Size) {}

		bool operator == (ElementTextureClass& other)
		{
			if(this->Shape_ != other.Shape_)
				return false;

			if(this->Size_ != other.Size_)
				return false;

			return true;
		}
	};

	static Map <Image> images_;

	static Map <DataBuffer> buffers_;

	static Map <Array <ElementTextureSet>, ElementTextureClass> textureSets_;

	static Shader * shader_;

	static void SetupKernels();

	static void SetupPaperGenerator(Size);

	static void Blur(Size);

	static void Crop(Size, Size, Size, ElementShapes);

	static void ConvertBlurToAlpha(Size);

	static void ConvertBlurToColor(Shader*, Size);

	static void HighlightEdges(Size, const char*);

	static void ApplyBrushes(Size);

	static void GenerateShadow(Size, Texture*&);

	static void GeneratePaper(Size, ElementShapes);

	static Texture* GenerateBaseTexture(Size, Size, Size, ElementShapes);

	static Texture* GenerateShadowTexture(Size);

	static void GeneratePaperBase(Size);

	static void SetStage(Stages);

public:
	static void Initialize();

	static void PaintInterface();

	static void GenerateTextures(ElementShapes, Size, Texture*&);

	static void GenerateStencils();

	static ElementTextureSet* GetTextureSet(ElementShapes, ElementSizes, Index);
};
