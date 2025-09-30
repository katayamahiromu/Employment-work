#pragma once
#include <d3d11.h>
#include <wrl.h>
#include<memory>
#include"Sprite.h"
#include"Camera.h"

class Skymap
{
public:
	Skymap(const wchar_t* filename);
	~Skymap();

	void Render(ID3D11DeviceContext* immediate_context,Camera&camera);

	struct sky_map_constants
	{
		DirectX::XMFLOAT4		camera_position;
		DirectX::XMFLOAT4X4		inverse_view_projection;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skymap_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> skymap_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skymap_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11Buffer>sky_map_buffer;
	D3D11_TEXTURE2D_DESC skymap_texture2d_desc;
	std::unique_ptr<Sprite>skymap_sprite;
};