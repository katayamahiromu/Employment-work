#include"Mask.h"
#include"DeviceManager.h"
#include"Shader.h"
#include"Buffer.h"
#include"imgui.h"

Mask::Mask()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	ShaderManager::instance()->createPsFromCso(device, "Shader//MaskPS.cso", pixelShader.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
	createBuffer<MaskData>(device, buffer.GetAddressOf());

	mask = std::make_unique<Sprite>(device,L"Resources\\Image\\dissolve_animation.png");
	noise = std::make_unique<Sprite>(device, L"Resources\\Image\\NoiseTexture1.png");
}

Mask::~Mask()
{
	
}

void Mask::update(float elapsedTime)
{
	data.time += elapsedTime;
}

void Mask::bindShader()
{
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	bindBufferToPostEffect<MaskData>(dc, 2, buffer.GetAddressOf(), &data);

	dc->PSSetShaderResources(1, 1, mask->getShaderResourceView().GetAddressOf());
	dc->PSSetShaderResources(2, 1, noise->getShaderResourceView().GetAddressOf());
}

void Mask::unBind()
{
	/*ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	dc->PSSetShaderResources(1, 1,nullptr);
	dc->PSSetShaderResources(2, 1,nullptr);*/
}

void Mask::debugGui()
{
	if (ImGui::TreeNode("Mask"))
	{
		ImGui::SliderFloat("dissolveThreshold", &data.dissolveThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("edgeThreshold", &data.edgeThreshold, 0.0f, 1.0f);

		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
}