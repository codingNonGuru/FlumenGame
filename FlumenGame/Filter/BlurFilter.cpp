#include <iostream>

#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"
#include "FlumenEngine/Render/BufferManager.hpp"
#include "FlumenEngine/Render/FrameBuffer.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Utility/Kernel.hpp"

#include "FlumenGame/Filter/BlurFilter.hpp"
#include "FlumenGame/Types.hpp"

void BlurFilter::HandleInitialize() 
{
	auto screen = Engine::GetScreen();

	swapBuffer_ = new FrameBuffer();
	swapBuffer_->Initialize(screen->GetSize(), FrameBufferAttachments::COLOR);

	kernel_ = new Kernel(60);
	kernel_->Initialize(KernelTypes::GAUSS, 0.0f);

	auto weights = kernel_->GetWeights();
	weightBuffer_ = new DataBuffer(weights.GetMemorySize(), weights.GetStart());
}

void BlurFilter::HandleUpdate(Camera* camera)
{
	float blurFactor = 0.01f + GetTimeFactor();
	blurFactor = 2.0f * pow(blurFactor * 30.0f, 2.0f);
	kernel_->Initialize(KernelTypes::GAUSS, blurFactor);

	auto weights = kernel_->GetWeights();
	weightBuffer_->UploadData(weights.GetStart(), weights.GetMemorySize());

	shader_->Bind();

	Matrix& matrix = camera->GetMatrix();
	shader_->SetConstant(matrix, "viewMatrix");

	auto textureSize = screenTexture_->GetSize();
	Scale2 scale = Scale2(textureSize.x, textureSize.y);
	shader_->SetConstant(scale, "screenSize");

	shader_->SetConstant(kernel_->GetSide(), "kernelSide");

	weightBuffer_->Bind(0);

	ApplyFirstPass(camera);

	ApplySecondPass(camera);

	shader_->Unbind();

	screenTexture_->Unbind();
}

void BlurFilter::ApplyFirstPass(Camera* camera)
{
	shader_->SetConstant(0, "mode");

	screenTexture_->Bind(0, shader_, "source");

	swapBuffer_->BindBuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void BlurFilter::ApplySecondPass(Camera* camera)
{
	shader_->SetConstant(1, "mode");

	auto defaultRenderTarget = BufferManager::GetFrameBuffer(FrameBuffers::DEFAULT);
	defaultRenderTarget->BindBuffer();

	swapBuffer_->BindTexture(shader_, "source");

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
