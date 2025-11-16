#include"CameraInfo.h"

CameraInfo::CameraInfo(Camera* camera)
{
	if (!camera)assert("camera is null");
	this->camera = camera;
}

CameraInfo::~CameraInfo()
{

}

void CameraInfo::prepare()
{

}

void CameraInfo::update(float elapsedTime)
{

}

void CameraInfo::OnGUI()
{

}