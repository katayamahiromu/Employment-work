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
	gaussian.texcel = {1280.0f, 720.0f};
}

GaussianFilter::~GaussianFilter()
{

}

void GaussianFilter::update(float elapsedTime)
{
	calcGaussianFilterConstant(gaussian, data);
}

void GaussianFilter::bindShader()
{
	bindBufferToPostEffect<Gaussian>(
		DeviceManager::instance()->getDeviceContext(), 2, buffer.GetAddressOf(), &gaussian);
}

void GaussianFilter::calcGaussianFilterConstant(Gaussian& constant, const GaussianDates& data)
{
	//	‹ô”‚Ìê‡‚ÍŠï”‚É’¼‚·
	int kernelSize = data.kernelSize;
	if (kernelSize % 2 == 0)
		kernelSize++;
	constant.kernelSize = kernelSize;
	constant.texcel.x = 1.0f / data.textureSize.x;
	constant.texcel.y = 1.0f / data.textureSize.y;
	//	d‚İ‚ğZo
	float sum = 0.0f;
	int id = 0;
	for (int y = -kernelSize / 2; y <= kernelSize / 2; y++)
	{
		for (int x = -kernelSize / 2; x <= kernelSize / 2; x++)
		{
			constant.weights[id].x = (float)x;
			constant.weights[id].y = (float)y;
			constant.weights[id].z = (float)exp(-(x * x + y * y) / (2.0f * data.sigma * data.sigma)) / (2.0f * DirectX::XM_PI * data.sigma);
			sum += constant.weights[id].z;
			id++;
		}
	}
	//	•½‹Ï‰»
	for (int i = 0; i < kernelSize * kernelSize; i++)
	{
		constant.weights[i].z /= sum;
	}
}

void GaussianFilter::debugGui()
{
	if (ImGui::TreeNode("Gaussian Filter"))
	{
		ImGui::SliderInt("kernelSize", &data.kernelSize, 1, 15);
		ImGui::SliderFloat("deviation", &data.sigma, 1.0f, 10.0f);
		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
}