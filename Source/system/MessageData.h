#pragma once
#include"math/Collision.h"

namespace MessageData
{
	//フリーカメラ
	static	constexpr const char* CAMERACHANGEFREEMODE = "CAMERA CHANGE FREEMODE";
	struct	CAMERACHANGEFREEMODEDATA
	{
		DirectX::XMFLOAT3	target;
		DirectX::XMFLOAT4	angle;
	};

	//レイキャスト
	static	constexpr const char* RAY_CAST_RESULT = "RAY CAST RESULT";
	struct RAYCASTREQUEST
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 end;
		HitResult result;
		bool isHit;
	};

	static constexpr const char* CYLINDER_INFORMATION = "cylinder information";
	struct CylinderInfo
	{
		DirectX::XMFLOAT3 pos;
		float range;
		float height;
	};

	static constexpr const char* REGISTER_COLLISIO_COMPONENT = "register_collision_component";
}
