#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"Audio/AudioManager.h"

//オーディオから受けとった波形をスペクトラムとして表示
class Spectrum
{
public:
	Spectrum(ID3D11Device*device,int numBins);
	~Spectrum();

    void update(Audio* audio);
	void draw(ID3D11DeviceContext* context);

    std::vector<float>* getSpectrumData() { return &spectrums; }
private:

    struct spectrumInfo
    {
        float numBins;
        float viewWidth;
        float viewHeight;
        float dummy;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer>spectrum_buffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;

    Microsoft::WRL::ComPtr<ID3D11Texture1D> spectrum_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

    std::vector<float>spectrums;
    int bin_count;
};
