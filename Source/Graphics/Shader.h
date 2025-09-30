#pragma once

#include <d3d11.h>

class ShaderManager
{
private:
	ShaderManager() {}
	~ShaderManager() {}
public:
	static ShaderManager* instance()
	{
		static ShaderManager inst;
		return &inst;
	}

	HRESULT createVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

	HRESULT createPsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	HRESULT createGsFromCso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** gemetory_shader);
};