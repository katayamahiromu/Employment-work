#include"RewidLine.h"
#include"Buffer.h"
#include"../DeviceManager.h"
#include"Shader.h"
#include"imgui.h"

RewindLine::RewindLine()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	createBuffer<RewindLineParameter>(device, buffer.GetAddressOf());
	ShaderManager::instance()->createPsFromCso(device, "Shader//RewindLinePS.cso", pixelShader.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, rlParam.screenSize.x,rlParam.screenSize.y);
}

RewindLine::~RewindLine()
{

}

void RewindLine::update(float elapsedTime)
{
	rlParam.time += elapsedTime;
	bindBufferToPostEffect<RewindLineParameter>(
		DeviceManager::instance()->getDeviceContext(), 2, buffer.GetAddressOf(), &rlParam);
}

void RewindLine::debugGui()
{
	if (ImGui::TreeNode("Rewind Line"))
	{
		ImGui::SliderFloat("Time",&rlParam.time, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	ImGui::Separator();
}