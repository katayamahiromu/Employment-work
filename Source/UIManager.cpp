#include"UIManager.h"
#include"Graphics/GraphicsManager.h"

UIManager::UIManager()
{

}

UIManager::~UIManager()
{
	clear();
}

void UIManager::update(float elapsedTime)
{
	for (UI* ui : UIArray)
	{
		ui->update(elapsedTime);
	}

	for (UI* ui : removeArray)
	{
		std::list<UI*>::iterator it = std::find(UIArray.begin(), UIArray.end(), ui);
		if (it != UIArray.end())
		{
			UIArray.erase(it);
		}
		delete ui;
	}

	removeArray.clear();
}

void UIManager::render(ID3D11DeviceContext* dc)
{
	GraphicsManager* graphics = GraphicsManager::instance();
	// 2D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i���j�A�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I�t�A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});

	for (UI* ui : UIArray)
	{
		ui->render(dc);
	}
}

void UIManager::clear()
{
	for (UI* ui : UIArray)
	{
		delete ui;
	}
	UIArray.clear();
}