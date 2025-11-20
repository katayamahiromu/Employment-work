#include"BlockToAlpha.h"
#include"Shader.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"../imgui/imgui.h"

BlackToAlpha::BlackToAlpha()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//BlackToAlphaPS.cso", pixelShader.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
}

void BlackToAlpha::debugGui()
{
	if (ImGui::TreeNode("Block To Alpha"))
	{
		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
}