#include"FFT-Cooley-Tukey.h"
#include<DirectXMath.h>
#include<algorithm>
#include"misc.h"
#include"Graphics/Shader.h"
#include"Graphics/Buffer.h"

void FFT::initialize(ID3D11Device* device)
{
	HRESULT hr{ S_OK };
	//ビット反転のコンピュートシェーダー準備
	{
		//createInputBuffer<Complex>(device, reversBit.structured_buffer[0].GetAddressOf(), 1024);
		//createInputBuffer<Complex>(device, reversBit.structured_buffer[1].GetAddressOf(), 1024);
		createOutputBuffer<Complex>(device, reversBit.structured_buffer[0].GetAddressOf(), 1024);
		createOutputBuffer<Complex>(device, reversBit.structured_buffer[1].GetAddressOf(), 1024);
		createOutputSRV(device,reversBit.structured_buffer[0].Get(), reversBit.shader_resource_view[0].GetAddressOf());
		createOutputSRV(device,reversBit.structured_buffer[1].Get(), reversBit.shader_resource_view[1].GetAddressOf());
		createUnorderedAccessView(device, reversBit.structured_buffer[0].Get(), reversBit.unordered_access_view[0].GetAddressOf());
		createUnorderedAccessView(device, reversBit.structured_buffer[1].Get(), reversBit.unordered_access_view[1].GetAddressOf());
		createCopyBuffer<Complex>(device, reversBit.output_copy_buffer.GetAddressOf(),1024);
		ShaderManager::instance()->createCsFromCso(device, "Shader//BitReverseCS.cso", reversBit.basic_compute_shader.GetAddressOf());
	}

	//バタフライ関数用のコンピュートシェーダー準備
	{
		//createInputBuffer<Complex>(device, butterfly.structured_buffer[0].GetAddressOf(), 1024);
		//createInputBuffer<Complex>(device, butterfly.structured_buffer[1].GetAddressOf(), 1024);
		createOutputBuffer<Complex>(device, butterfly.structured_buffer[0].GetAddressOf(), 1024);
		createOutputBuffer<Complex>(device, butterfly.structured_buffer[1].GetAddressOf(), 1024);
		createOutputSRV(device, butterfly.structured_buffer[0].Get(), butterfly.shader_resource_view[0].GetAddressOf());
		createOutputSRV(device, butterfly.structured_buffer[1].Get(), butterfly.shader_resource_view[1].GetAddressOf());
		createUnorderedAccessView(device, butterfly.structured_buffer[0].Get(), butterfly.unordered_access_view[0].GetAddressOf());
		createUnorderedAccessView(device, butterfly.structured_buffer[1].Get(), butterfly.unordered_access_view[1].GetAddressOf());
		createCopyBuffer<Complex>(device, butterfly.output_copy_buffer.GetAddressOf(),1024);
		ShaderManager::instance()->createCsFromCso(device, "Shader//ButterflyCS.cso", butterfly.basic_compute_shader.GetAddressOf());
	}

	//共通のバッファを作成
	createBuffer<FFTBuffer>(device, fftBuffer.GetAddressOf());
}

size_t FFT::nextPow2(size_t n)
{
	size_t p = 1;
	while (p < n)p <<= 1;
	return p;
}

void FFT::bitReverse(std::vector<Complex>& data)
{
	const size_t n = data.size();
	size_t j = 0;
	for (size_t i = 1;i < n;++i)
	{
		size_t bit = n >> 1;
		for (; j & bit; bit >>= 1) j ^= bit;
		j ^= bit;
		if (i < j) std::swap(data[i], data[j]);
	}
}

void FFT::fft(std::vector<Complex>& data)
{
	const size_t n = data.size();
	if (n <= 1)return;

	bitReverse(data);

	for (size_t len = 2;len <= n;len <<= 1)
	{
		const float ang = -2.0f * static_cast<float>(DirectX::XM_PI) / static_cast<float>(len);
		Complex wlen = Complex(std::cos(ang), std::sin(ang));
		for (size_t i = 0;i < n;i += len)
		{
			Complex w = 1.0f;

			for (size_t j = 0;j < len / 2;++j)
			{
				Complex u = data[i + j];
				Complex v = data[i + j + len / 2] * w;
				data[i + j] = u + v;
				data[i + j + len / 2] = u - v;
				w *= wlen;
			}
		}
	}
}

void FFT::ifft(std::vector<Complex>& data)
{
	size_t n = data.size();

	//入力を複素共役に
	for (auto& i : data)
		i = std::conj(i);

	//FFTを実行（通常の正方向）
	fft(data);

	//出力を再び複素共役に
	for (auto& i : data)
		i = std::conj(i);

	//正規化
	for (auto& i : data)
		i /= static_cast<float>(n);
}

std::vector<Complex> FFT::window(const std::vector<Complex>& src)
{
	std::vector<Complex> dst = src;
	size_t n = src.size();
	for (size_t i = 0;i < n;++i)
	{
		float w = 0.54f - 0.46f * std::cos(2.0f * static_cast<float>(DirectX::XM_PI) * i / static_cast<float>(n - 1));
		dst[i] *= w;
	}

	return dst;
}

std::vector<Complex> FFT::fft_from_uint8(const std::vector<UINT8>& input, bool applyWindow)
{
	std::vector<Complex> buf = ConvertBuffer<UINT8>(input);

	//窓を適用
	if (applyWindow) buf = window(buf);

	fft(buf);

	return buf; //複素スペクタル（長さ　n）
}

std::vector<Complex> FFT::fft_from_uint8(ID3D11DeviceContext* dc, const std::vector<UINT8>& input, bool applyWindow)
{
	std::vector<Complex> buf = ConvertBuffer<UINT8>(input);

	//窓を適用
	if (applyWindow) buf = window(buf);

	auto reset = [&]()->void
	{
		// 同一リソースを同時に SRV/UAV にしないようにする
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
		dc->CSSetShaderResources(0, 1, nullSRV);
		dc->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	};

	//バッファーの更新
	FFTBuffer param;
	param.N = static_cast<unsigned int>(buf.size());
	param.log2N = static_cast<UINT>(std::log2(static_cast<float>(param.N)));
	dc->UpdateSubresource(fftBuffer.Get(), 0, 0, &param, 0, 0);

	const UINT THREADS_PER_GROUP = 16;
	UINT groupCount = static_cast<UINT>((buf.size() + THREADS_PER_GROUP - 1) / THREADS_PER_GROUP);
	
	//ビット反転をGPU側で計算
	{
		//GPUへのデータの転送と関数の実行
		dc->UpdateSubresource(reversBit.structured_buffer[0].Get(), 0, nullptr, buf.data(), 0, 0);
		dc->CSSetConstantBuffers(10, 1, fftBuffer.GetAddressOf());
		dc->CSSetShaderResources(0, 1, reversBit.shader_resource_view[0].GetAddressOf());
		dc->CSSetUnorderedAccessViews(0, 1, reversBit.unordered_access_view[1].GetAddressOf(), nullptr);
		dc->CSSetShader(reversBit.basic_compute_shader.Get(), nullptr, 0);
		dc->Dispatch(groupCount, 1, 1);

		//アンバインド
		reset();
	}

	//バタフライ関数を実行
	{
		int readIndex = 0; //SRVをセットするバッファ
		int writeIndex = 1; //UAVをセットするバッファ

		dc->CopyResource(butterfly.structured_buffer[readIndex].Get(), reversBit.structured_buffer[1].Get());

		for (unsigned int stage = 0;stage < param.log2N;++stage)
		{
			//GPUへのデータの転送と関数の実行
			param.stage = stage;
			dc->UpdateSubresource(fftBuffer.Get(), 0, nullptr, &param, 0, 0);

			//新しい入力をセット
			dc->CSSetShaderResources(0, 1, butterfly.shader_resource_view[readIndex].GetAddressOf());
			dc->CSSetUnorderedAccessViews(0, 1, butterfly.unordered_access_view[writeIndex].GetAddressOf(), nullptr);

			//実行
			dc->CSSetConstantBuffers(10, 1, fftBuffer.GetAddressOf());
			dc->CSSetShader(butterfly.basic_compute_shader.Get(), nullptr, 0);
			dc->Dispatch(groupCount, 1, 1);

			// アンバインド
			reset();

			// 入出力を入れ替える
			std::swap(readIndex, writeIndex);
		}

		//計算の受け取り
		dc->CopyResource(butterfly.output_copy_buffer.Get(), butterfly.structured_buffer[readIndex].Get());
		D3D11_MAPPED_SUBRESOURCE sub_resource;
		HRESULT hr = dc->Map(butterfly.output_copy_buffer.Get(), 0, D3D11_MAP_READ, 0, &sub_resource);
		if (SUCCEEDED(hr))
		{
			size_t dataSize = buf.size() * sizeof(Complex);
			memcpy(buf.data(), sub_resource.pData, dataSize);
			dc->Unmap(butterfly.output_copy_buffer.Get(), 0);
		}
	}
	return buf;
}

std::vector<float> FFT::magnitude_spectrum(const std::vector<Complex>& spectrum)
{
	size_t n = spectrum.size();
	std::vector<float>mag(n / 2 + 1);
	for (size_t i = 0;i <= n / 2;++i)
	{
		mag[i] = std::abs(spectrum[i]);
	}
	return mag;
}

std::vector<float> FFT::magnitude_db(const std::vector<float>& mag, float ref)
{
	const float eps = 1e-12f;
	std::vector<float>db(mag.size());
	for (size_t i = 0;i < mag.size();++i)
	{
		db[i] = 20.0f * std::log10(max(mag[i], eps) / ref);
	}
	return db;
}