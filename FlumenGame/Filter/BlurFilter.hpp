#pragma once

#include "FlumenEngine/Render/Filter.hpp"

class Camera;
class FrameBuffer;
class DataBuffer;
class Kernel;

class BlurFilter : public Filter
{
	FrameBuffer* swapBuffer_;

	DataBuffer* weightBuffer_;

	Kernel* kernel_;

	void ApplyFirstPass(Camera*);

	void ApplySecondPass(Camera*);

protected:
	virtual void HandleInitialize() override;

	virtual void HandleUpdate(Camera*) override;
};
