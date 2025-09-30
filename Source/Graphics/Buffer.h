#pragma once

#include <d3d11.h>
#include <wrl.h>
#include"BufferSlots.h"

/// <summary>
/// 定数バッファの作成
/// </summary>
/// <param name="buffer">生成する定数バッファ</param>
template<typename Ty>
inline HRESULT createBuffer(ID3D11Device* device, ID3D11Buffer** buffer)
{
	// シーン用定数バッファ
	D3D11_BUFFER_DESC desc;
	::memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(Ty);
	desc.StructureByteStride = 0;

	return device->CreateBuffer(&desc, 0, buffer);
}

/// <summary>
/// 定数バッファの更新と設定
/// </summary>
/// <param name="dc">デバイスコンテキスト</param>
/// <param name="buffer">更新される定数バッファ</param>
/// <param name="constants">更新する定数のデータ</param>
template<typename Ty>
inline void bindBuffer(ID3D11DeviceContext* dc, int slot, ID3D11Buffer** buffer, Ty* constants)
{
	// 定数バッファの登録
	dc->UpdateSubresource(*buffer, 0, 0, constants, 0, 0);
	dc->VSSetConstantBuffers(slot, 1, buffer);
	dc->PSSetConstantBuffers(slot, 1, buffer);
	dc->GSSetConstantBuffers(slot, 1, buffer);
}

/// <summary>
/// ポストエフェクト用の定数バッファの更新と設定
/// </summary>
/// <param name="dc">デバイスコンテキスト</param>
/// <param name="buffer">更新される定数バッファ</param>
/// <param name="constants">更新する定数のデータ</param>
template<typename Ty>
inline void bindBufferToPostEffect(ID3D11DeviceContext* dc, int slot, ID3D11Buffer** buffer, Ty* constants)
{
	// 定数バッファの登録
	dc->UpdateSubresource(*buffer, 0, 0, constants, 0, 0);
	dc->PSSetConstantBuffers(slot, 1, buffer);
}