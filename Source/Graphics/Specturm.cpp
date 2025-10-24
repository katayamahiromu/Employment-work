#include"Specturm.h"
#include "misc.h"
#include "Shader.h"
#include"Buffer.h"
#include"math/FFT-Cooley-Tukey.h"
#include"DeviceManager.h"
#include"imgui.h"

Spectrum::Spectrum(ID3D11Device* device,int numBins):bin_count(numBins)
{
	//シェーダーの読み込み
	ShaderManager::instance()->createVsFromCso(device, ".\\Shader\\SpectrumVS.cso", vertex_shader.GetAddressOf(), nullptr, {}, 0);
	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\SpectrumPS.cso", pixel_shader.GetAddressOf());

	//テクスチャの生成
	D3D11_TEXTURE1D_DESC desc = {};
	desc.Width = numBins;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ArraySize = 1;

	device->CreateTexture1D(&desc, nullptr, &spectrum_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = desc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	srvd.Texture1D.MipLevels = 1;
	device->CreateShaderResourceView(spectrum_texture.Get(), &srvd, &shader_resource_view);

	//バッファーの作成
	createBuffer<spectrumInfo>(device, spectrum_buffer.GetAddressOf());

	FFT::instance()->initialize(device);
}

Spectrum::~Spectrum()
{

}

void Spectrum::update(Audio* audio)
{
	const int FFT_SIZE = 1024; //解析サイズ

	size_t startPosition = audio->getSamplePlay(); //再生中のサンプル位置
	AudioResource* resource = audio->getResource(); //WAVデータ

	//WAVデータ全体から現在の再生位置を基準にFFT_SIZE分だけ切り出す
	const std::vector<UINT8> audioData = resource->allData();
	size_t totalSamples = resource->getAudioBytes(); //バイト数

	// 再生位置が末尾に近い場合は安全にゼロ埋め
	std::vector<UINT8>data(FFT_SIZE, 0);
	for (size_t i = 0; i < FFT_SIZE && (startPosition + i) < totalSamples; ++i)
	{
		data[i] = audioData[startPosition + i];
	}

	auto complex = gpCpu ?
		FFT::instance()->fft_from_uint8(DeviceManager::instance()->getDeviceContext(), data):
		FFT::instance()->exitFFT<UINT8>(data);

	auto mag = FFT::instance()->magnitude_spectrum(complex);
	std::vector<float> half(mag.begin(), mag.begin() + FFT_SIZE / 2);
	auto db = FFT::instance()->magnitude_db(half, 1.0f);

	// ③ dB → 0〜1に正規化
	std::vector<float> normalized(db.size());
	const float minDb = -200.0f;
	const float maxDb = 200.0f;
	for (size_t i = 0; i < db.size(); ++i)
	{
		float value = (db[i] - minDb) / (maxDb - minDb);
		normalized[i] = std::clamp(value, 0.0f, 1.0f);
	}

	spectrums = normalized;
}

void Spectrum::OnGUi()
{
	ImGui::Checkbox("Change GPU or CPU", &gpCpu);
	ImGui::Text("Current Mode: %s", gpCpu ? "GPU" : "CPU");
}
void Spectrum::draw(ID3D11DeviceContext* context)
{
	HRESULT hr{ S_OK };

    // スペクトラムテクスチャの更新
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (SUCCEEDED(context->Map(spectrum_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        // FFTの結果をテクスチャにコピー
        memcpy(mapped.pData, spectrums.data(), sizeof(float) * bin_count);
        context->Unmap(spectrum_texture.Get(), 0);
    }

    //定数バッファの更新
    spectrumInfo info;
    info.numBins = static_cast<float>(bin_count);
    info.viewWidth = 1280.0f;
    info.viewHeight = 720.0f;
    bindBuffer<spectrumInfo>(context, 10, spectrum_buffer.GetAddressOf(), &info);

    //描画パイプラインの設定
    context->IASetInputLayout(nullptr); // 頂点バッファを使用しないため
    context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // シェーダーの設定
    context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //リソースのバインド
    ID3D11ShaderResourceView* srvs[] = { shader_resource_view.Get() };
    context->VSSetShaderResources(0, 1, srvs);
    context->PSSetShaderResources(0, 1, srvs);

    //描画実行 (bin_count * 6 は全ビンの全頂点を生成)
    context->Draw(bin_count * 6, 0);

    //描画後のリソースのアンバインド
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    context->PSSetShaderResources(0, 1, nullSRV);
}