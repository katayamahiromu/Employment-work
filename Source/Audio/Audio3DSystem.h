#pragma once
#include<DirectXMath.h>
#include<vector>
#include"Audio.h"

enum class Lisner:int
{
	PLAYER,
	CAERA
};

//音源
struct SoundEmitter
{
	DirectX::XMFLOAT3 position = {0.0f,0.0f,0.0f};
	DirectX::XMFLOAT3 velocity = {0.0f,0.0f,0.0f};
	float minDistance;
	float maxDistance;
};

//リスナー
struct SoundListner
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 frontVec;
	DirectX::XMFLOAT3 rightVec;
	float innerRadius;
	float outerRadius;
	float filterParam;
};

struct SoundDSPSetting
{
	int srcChannelCount;
	int dstChannelCount;
	float distanceListerEmitter;
	float dopplerScale;
	float radianListerEmitter;
	float *outputMatrix;
	float filterParam;
};

class Audio3D : public Audio
{
public:
	Audio3D(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, SoundEmitter* emitter);
	~Audio3D();

	void update(float volume);
	void setDSPSetting(SoundListner& listner);

	void gui();
private:
	void setPan();
	void filter(XAUDIO2_FILTER_TYPE type, FLOAT32 overq = 1.0f);
	float calcAngle(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b,DirectX::XMFLOAT3 vec);
private:
	SoundEmitter*emitter;
	SoundDSPSetting dspSetting;
	float absorptionRata;
	float reflectionRate;

	std::vector<X3DAUDIO_DISTANCE_CURVE_POINT>volCurvePoints;
	X3DAUDIO_DISTANCE_CURVE volCurve = { nullptr,0 };

	//直接音に適用するローパスフィルターの強さを、距離に応じて変化させるためのカーブ
	std::vector<X3DAUDIO_DISTANCE_CURVE_POINT>lpdDirectCurvePoints;
	X3DAUDIO_DISTANCE_CURVE lpfDirectCurve = { nullptr,0 };

	//音の減衰率
	float scale = 1.0f;
	float left, right;
};