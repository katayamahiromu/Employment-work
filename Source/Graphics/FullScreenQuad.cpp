#include"FullScreenQuad.h"
#include"Shader.h"

FullScreenQuad::FullScreenQuad(ID3D11Device* dc)
{
	ShaderManager* sm = ShaderManager::instance();
	sm->createVsFromCso(dc, "Shader//FullScreenQuadVS.cso", embeddedVertexShader.GetAddressOf(), nullptr, nullptr, 0);
	sm->createPsFromCso(dc, "Shader//FullScreenQuadPS.cso", embeddedPixelShader.GetAddressOf());
}

void FullScreenQuad::bilt(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader)
{
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(nullptr);

	dc->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ?
		dc->PSSetShader(replacedPixelShader, 0, 0) :
		dc->PSSetShader(embeddedPixelShader.Get(), 0, 0);
	dc->PSSetShaderResources(startSlot, numViews, shaderResourceView);
	dc->Draw(4, 0);
}