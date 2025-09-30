#include"Skymap.h"
#include"Texture.h"
#include"misc.h"
#include"Shader.h"
#include"Framework.h"
#include"Buffer.h"
#include"DeviceManager.h"
#include"GraphicsManager.h"


Skymap::Skymap(const wchar_t* filename)
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	//�X�J�C�}�b�v�p�̃e�N�X�`���y�уX�v���C�g������
	TextureManager::instance()->loadTextureFromFile(
		device, filename,
		skymap_shader_resource_view.GetAddressOf(), &skymap_texture2d_desc
	);

	skymap_sprite = std::make_unique<Sprite>(device, skymap_shader_resource_view);

	//�V�F�[�_�[�̓ǂ݂���
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ShaderManager* shader = ShaderManager::instance();
	shader->createVsFromCso(device, "Shader/SkymapVS.cso", skymap_vertex_shader.GetAddressOf(), skymap_input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
	shader->createPsFromCso(device, "Shader/SkymapPS.cso", skymap_pixel_shader.GetAddressOf());

	createBuffer<Skymap::sky_map_constants>(device, sky_map_buffer.GetAddressOf());
}

Skymap::~Skymap()
{

}

void Skymap::Render(ID3D11DeviceContext* immediate_context,Camera& camera)
{
	GraphicsManager* graphics = GraphicsManager::instance();

	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});

	//�V�F�[�_�[�ݒ�
	immediate_context->IASetInputLayout(skymap_input_layout.Get());
	immediate_context->VSSetShader(skymap_vertex_shader.Get(),nullptr,0);
	immediate_context->PSSetShader(skymap_pixel_shader.Get(), nullptr, 0);

	//�o�b�t�@�[�̃o�C���h
	DirectX::XMFLOAT3 pos = *camera.getEye();
	sky_map_constants smc;
	smc.camera_position.x = pos.x;
	smc.camera_position.y = pos.y;
	smc.camera_position.z = pos.z;
	smc.camera_position.w = 1.0;

	DirectX::XMStoreFloat4x4(&smc.inverse_view_projection, 
		DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(camera.getView()) * DirectX::XMLoadFloat4x4(camera.getProjection())));

	bindBuffer<sky_map_constants>(immediate_context, CBS_SKY_MAP, sky_map_buffer.GetAddressOf(), &smc);

	//�`��
	skymap_sprite->render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}