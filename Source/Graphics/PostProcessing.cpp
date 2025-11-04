#include"PostProcessing.h"
#include"DeviceManager.h"

PostProcess::PostProcess()
{
	framebuffer = std::make_unique<FrameBuffer>(DeviceManager::instance()->getDevice(), 1280, 720);
}
void PostProcess::prepare(ID3D11DeviceContext* dc)
{
	framebuffer->clear(dc);
	framebuffer->activate(dc);
}

void PostProcess::prepare(ID3D11DeviceContext* dc, std::vector<ID3D11RenderTargetView*>rtvs)
{
	framebuffer->clear(dc,rtvs);
	framebuffer->activate(dc, rtvs);
}

void PostProcess::clean(ID3D11DeviceContext* dc)
{
	framebuffer->deactivate(dc);
}