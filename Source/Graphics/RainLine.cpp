#include"RainLine.h"
#include"Shader.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"imgui.h"

RainLine::RainLine()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//RainPS.cso", pixelShader.GetAddressOf());
	createBuffer<RainParam>(device, buffer.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
}

RainLine::~RainLine()
{

}

void RainLine::update(float elapsedTime)
{
	param.time += elapsedTime;
}

void RainLine::bindShader()
{
	bindBufferToPostEffect<RainParam>(
		DeviceManager::instance()->getDeviceContext(), 2, buffer.GetAddressOf(), &param);
}

void RainLine::debugGui()
{
	if (ImGui::TreeNode("Rain Param"))
	{
		ImGui::SliderFloat("Intensity", &param.rainIntensity, 0.0f, 5.0f);
		ImGui::SliderFloat("Speed", &param.rainSpeed, 0.1f, 10.0f);
		ImGui::SliderFloat("Scale", &param.rainScale, 1.0f, 50.0f);
		ImGui::TreePop();
	}
	ImGui::Separator();
}