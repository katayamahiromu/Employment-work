#pragma once
#include<DirectXMath.h>

//カメラ
class Camera
{
private:
	Camera() {}
	~Camera(){}
public:
	static Camera* instance()
	{
		static Camera inst;
		return &inst;
	}
	//デフォルト設定
	void defaultSetting();

	//カメラの位置と姿勢の設定
	void setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& forcus,const DirectX::XMFLOAT3& up);

	//カメラのレンズの設定
	void setPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	//ビュー行列取得
	const DirectX::XMFLOAT4X4* getView()const { return &view; }

	//プロジェクション行列
	const DirectX::XMFLOAT4X4* getProjection()const { return &projection; }

	//視点取得
	const DirectX::XMFLOAT3* getEye()const { return &eye; }

	//注視点取得
	const DirectX::XMFLOAT3* getFocus()const { return &focus; }

	//右方向ベクトル取得
	const DirectX::XMFLOAT3* getRight()const { return &right; }

	//上方向ベクトル取得
	const DirectX::XMFLOAT3* getUp()const { return &up; }

	//前方向ベクトル
	const DirectX::XMFLOAT3* getFront()const { return &front; }

	void setEye(DirectX::XMFLOAT3* eye) { this->eye = *eye; }
private:
	DirectX::XMFLOAT4X4 view = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	DirectX::XMFLOAT4X4 projection = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	DirectX::XMFLOAT3 eye	= { 0,0,0 };	//カメラの視点（カメラ位置）
	DirectX::XMFLOAT3 focus = { 0,0,0 };	//注視

	DirectX::XMFLOAT3 right = { 0,0,0 };	//右方向ベクトル
	DirectX::XMFLOAT3 up	= { 0,0,0 };	//上方向ベクトル
	DirectX::XMFLOAT3 front = { 0,0,0 };	//前方向ベクトル
};