#include"LuminanceExtract.h"
#include"Shader.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"imgui.h"

LuminanceExtract::LuminanceExtract()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//LuminanceExtractPS.cso", pixelShader.GetAddressOf());
	createBuffer<luminanceExtractConstants>(device, buffer.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
};

LuminanceExtract::~LuminanceExtract()
{

}

void LuminanceExtract::debugGui()
{
	if (ImGui::TreeNode("Extract Luminance"))
	{
		ImGui::SliderFloat("threshold", &luminanceExtractConstant.threshold, 0.0f, 2.0f);
		ImGui::SliderFloat("intensity", &luminanceExtractConstant.intensity, 0.0f, 10.0f);
		ImGui::Image(framebuffer->getSrvP(), {128, 128}, {0, 0}, {1, 1}, {1, 1, 1, 1});
		ImGui::TreePop();
	}
	ImGui::Separator();
}

void LuminanceExtract::update(float elapsedTime)
{
	bindBufferToPostEffect<luminanceExtractConstants>(
		DeviceManager::instance()->getDeviceContext(), 2, buffer.GetAddressOf(), &luminanceExtractConstant);
}