#include"ACES_Filmic.h"
#include"Shader.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"../imgui/imgui.h"

ACES_Filmic::ACES_Filmic()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//ACES_FilmicPS.cso", pixelShader.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
}

void ACES_Filmic::debugGui()
{
	if (ImGui::TreeNode("Toon map ACES Filmic"))
	{
		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
}