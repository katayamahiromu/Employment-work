#include"ColorGrading.h"
#include"Shader.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"imgui.h"

ColorGrading::ColorGrading()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//ColorGrandingPS.cso", pixelShader.GetAddressOf());
	createBuffer<ColorGrading::CBColorGrading>(device, buffer.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
}

ColorGrading::~ColorGrading()
{

}

void ColorGrading::update(float elapsedTime)
{
	bindBufferToPostEffect<ColorGrading::CBColorGrading>(
		DeviceManager::instance()->getDeviceContext(), 2, buffer.GetAddressOf(), &colorGrading);
}

void ColorGrading::debugGui()
{
	if (ImGui::TreeNode("Color Grading"))
	{
		ImGui::SliderFloat("hueShift", &colorGrading.hueShift, 0.0f, +360.0f);
		ImGui::SliderFloat("saturation", &colorGrading.saturation, 0.0f, +2.0f);
		ImGui::SliderFloat("brightness", &colorGrading.brightness, 0.0f, +2.0f);

		//ImGui::DragFloat4("param", &colorGrading.hueShift);
		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
}