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
	std::vector<Complex> fft_from_uint8(ID3D11DeviceContext* dc, const std::vector<UINT8>& input, bool applyWindow = true);


	template<typename Ty>
	std::vector<Complex> exitFFT(const std::vector<Ty>& input, bool applyWindow = true)
	{
		std::vector<Complex> buf = ConvertBuffer<Ty>(input);

		//窓を適用
		if (applyWindow) buf = window(buf);

		fft(buf);

		return buf; //複素スペクタル（長さ　n）
	}

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

	//逆フーリエ変換
	void ifft(std::vector<Complex>& data);
	//窓関数
	std::vector<Complex>window(const std::vector<Complex>& src);

	//データをFFTを出来る形に直す
	template<typename T>
	std::vector<Complex>ConvertBuffer(std::vector<T>data)
	{
		if (data.empty())return {};

		size_t n_in = data.size();
		size_t n = nextPow2(n_in);

		// 準備：複素配列に変換（中心化して正規化）
		std::vector<Complex>buf(n, Complex(0.0f, 0.0f));
		for (size_t i = 0;i < n_in;++i)
		{
			// 8bit unsigned PCM -> 中心を 128 にして [-1, 1] に正規化
			float sample = (static_cast<int>(data[i] - 128)) / 128.0f;
			buf[i] = Complex(sample, 0.0f);
		}

		return buf;
	};

private:
	struct GPCPU
	{
		//	計算用シェーダー
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> basic_compute_shader;

		Microsoft::WRL::ComPtr<ID3D11Buffer> structured_buffer[2];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view[2];
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unordered_access_view[2];
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