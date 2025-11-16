#include"Camera.h"
#include"DeviceManager.h"

void Camera::defaultSetting()
{
	DeviceManager* deviceMgr = DeviceManager::instance();
	setLookAt(
		DirectX::XMFLOAT3(0, 10, -10),// カメラの視点（位置）
		DirectX::XMFLOAT3(0, 0, 0),	  // カメラの注視点（ターゲット）
		DirectX::XMFLOAT3(0, 1, 0)    // カメラの上方向
	);

	setPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 視野角
		deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// 画面縦横比率
		0.1f,		// カメラが映し出すの最近距離
		1000.0f		// カメラが映し出すの最遠距離
	);
}

//カメラの位置と姿勢の設定
void Camera::setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& forcus, const DirectX::XMFLOAT3& up)
{
	//視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&forcus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	//ビュー行列を逆行列計算し、ワールド行列に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//カメラの右方向を取り出す
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	//カメラの上方向を取り出す
	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	//カメラの前方向を取り出す
	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	//視点、注視点を保存
	this->eye = eye;
	this->focus = forcus;
}

//カメラのレンズの設定
void Camera::setPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	//視野角、縦横比率、クリップ距離からプロジェクション行列を作成
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}