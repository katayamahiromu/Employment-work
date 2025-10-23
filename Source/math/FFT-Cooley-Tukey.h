#pragma once
#include<vector>
#include<complex>
#include<cmath>
#include<windows.h>
#include<d3d11.h>
#include<wrl.h>

using Complex = std::complex<float>;

class FFT
{
public:
	FFT() {}
	~FFT(){}

	static FFT* instance()
	{
		static FFT inst;
		return &inst;
	}

	/// <summary>
	/// コンピュートシェーダーを使用するための準備
	/// </summary>
	void initialize(ID3D11Device* device);

	// UINT8データからFFTを実行して複素スペクトルを返す
	// input: unsigned 8-bit PCM (0..255) - 中心は128（unsigned PCM仕様）を想定
	// output: 複素スペクトル（0..N-1）を返す。出力長は input を次の2^kにゼロパディングした長さ。
	// applyWindow: ハミング窓を掛けるかどうか（デフォルト true）
	std::vector<Complex> fft_from_uint8(const std::vector<UINT8>& input, bool applyWindow = true);
	std::vector<Complex> fft_from_uint8(ID3D11DeviceContext*dc,const std::vector<UINT8>& input, bool applyWindow = true);

	//ユーティリティ: 複素スペクトルから振幅スペクトル（線形）を計算
	std::vector<float> magnitude_spectrum(const std::vector<Complex>& spectrum);

	// ユーティリティ: 振幅スペクトルを dB に変換（小さな値を避けるため epsilon）
	std::vector<float> magnitude_db(const std::vector<float>& mag, float ref = 1.0f);
private:
	//次の２のべき乗を得る
	size_t nextPow2(size_t n);

	//ビット反転インデックス変換
	void bitReverse(std::vector<Complex>& data);

	//高速フーリエ変換
	void fft(std::vector<Complex>& data);

	//窓関数
	std::vector<Complex>window(const std::vector<Complex>& src);

	//UINT8型のデータをFFTを出来る形に直す
	std::vector<Complex>ConvertBufferUint8(std::vector<UINT8>data);
private:
	struct GPCPU
	{
		//	計算用シェーダー
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> basic_compute_shader;
		//	計算用の情報を受け渡すためのバッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> input_structured_buffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> input_shader_resource_view;
		//	計算した結果を書き込むためのバッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> output_structured_buffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> output_unordered_access_view;
		//	出力結果をCPU側で受け取るためのコピーバッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> output_copy_buffer;
	};

	GPCPU butterfly;
	GPCPU reversBit;

	struct FFTBuffer
	{
		unsigned int N;
		unsigned int log2N;
		unsigned int stage;
		int dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>fftBuffer;
};