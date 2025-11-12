#include"GaussianFiltering.h"
#include"Shader.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"imgui.h"

GaussianFilter::GaussianFilter()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//GaussianFilterPS.cso", pixelShader.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
	createBuffer<GaussianFilter::Gaussian>(device, buffer.GetAddressOf());
	gaussian.texelSize = {1280.0f, 720.0f};
	settingDirection();
}

GaussianFilter::~GaussianFilter()
{

}

void GaussianFilter::update(float elapsedTime)
{

}

void GaussianFilter::bindShader()
{
	bindBufferToPostEffect<Gaussian>(
		DeviceManager::instance()->getDeviceContext(), 2, buffer.GetAddressOf(), &gaussian);
}

void GaussianFilter::settingDirection()
{
	switch (mode)
	{
	case 0:
		gaussian.sampleCount = 3;
		gaussian.directions[0].dir = { -1, 0 };
		gaussian.directions[1].dir = { 0, 0 };
		gaussian.directions[2].dir = { 1, 0 };
		break;
	case 1:
		gaussian.sampleCount = 5;
		gaussian.directions[0].dir = { 0, -1 };
		gaussian.directions[1].dir = { 0,  0 };
		gaussian.directions[2].dir = { 0,  1 };
		gaussian.directions[3].dir = { -1, 0 };
		gaussian.directions[4].dir = { 1, 0 };
		break;
	case 2:
		gaussian.sampleCount = 8;
		gaussian.directions[0].dir = { -1, -1 };
		gaussian.directions[1].dir = { -1,  1 };
		gaussian.directions[2].dir = { 1, -1 };
		gaussian.directions[3].dir = { 1,  1 };
		gaussian.directions[4].dir = { -1,  0 };
		gaussian.directions[5].dir = { 1,  0 };
		gaussian.directions[6].dir = { 0, -1 };
		gaussian.directions[7].dir = { 0,  1 };
		break;
	}
}

void GaussianFilter::debugGui()
{
	if (ImGui::TreeNode("Gaussian Filter"))
	{
		ImGui::InputFloat2("texel Size", &gaussian.texelSize.x);
		if (ImGui::SliderInt("offset", &power, 0, 10))
		{
			gaussian.offset = static_cast<float>(1 << power);
		}

		if (ImGui::SliderInt("mode", &mode, 0, 2))
		{
			settingDirection();
		}
		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
}