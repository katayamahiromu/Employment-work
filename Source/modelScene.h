#pragma once
#include"Scene.h"
#include"Graphics/SkinnedMesh.h"
#include"Component/object.h"
#include"DebugCameraController.h"
#include"Component/Animation.h"
#include"Component/meshTexchange.h"
#include"Graphics/Sprite.h"

class ModelScene : public Scene
{
public:
	ModelScene();
	~ModelScene();

	void initialize()override;

	void finalize()override;

	void update(float elaspedTime)override;

	void render()override;

	void Gui();
private:
	DirectX::XMFLOAT3 scale = {0.1f,0.1f,0.1f};
	DirectX::XMFLOAT3 position = {0.0f,0.0f,0.0f};
	DirectX::XMFLOAT4 rotation = {0.0f,0.0f,0.0f,0.0f};
private:
	std::unique_ptr<DebugCameraController>cameraController;
	std::unique_ptr<ObjectManager>objManager;
	std::shared_ptr<Object>model;


	std::vector<uint64_t>idArray;
	std::vector<std::unique_ptr<Sprite>>tex;
	std::shared_ptr<Animation>animation;
	std::shared_ptr<MeshTexChange>meshTexChange;
	int animationIndex = 0;
};