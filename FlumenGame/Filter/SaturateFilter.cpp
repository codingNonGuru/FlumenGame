#include "SaturateFilter.hpp"

#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Texture.hpp"

void SaturateFilter::HandleUpdate(Camera* camera)
{
	shader_->Bind();

	Matrix& matrix = camera->GetMatrix();
	shader_->SetConstant(matrix, "viewMatrix");

	auto textureSize = screenTexture_->GetSize();
	Scale2 scale = Scale2(textureSize.x, textureSize.y);
	shader_->SetConstant(scale, "screenSize");

	float factor = GetTimeFactor();
    shader_->SetConstant(factor, "factor");

	screenTexture_->Bind(0, shader_, "diffuse");

	glDrawArrays(GL_TRIANGLES, 0, 6);

	shader_->Unbind();

	screenTexture_->Unbind();
}