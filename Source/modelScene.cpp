#include"modelScene.h"
#include"Camera.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"imgui.h"

ModelScene::ModelScene()
{

}

ModelScene::~ModelScene()
{

}

void ModelScene::initialize()
{
	objManager = std::make_unique<ObjectManager>();

	//ƒJƒƒ‰‰ŠúÝ’è
	Camera::instance()->defaultSetting();
	cameraController = std::make_unique<CameraController>();

	//Œ©‚½‚¢‚â‚Â‚Ìƒ‚ƒfƒ‹
	model = objManager->create();
	model->loadModel("Resources\\Model\\pico\\pico_chan.fbx");
	model->setScale(scale);
	model->setPosition(position);
	model->setRotation(rotation);

	model->AddComponent<Animation>();

	animation = model->GetComponent<Animation>();
}

void ModelScene::finalize()
{
	objManager->clear();
}

void ModelScene::update(float elapsedTime)
{
	model->setScale(scale);
	model->setPosition(position);
	model->setRotation(rotation);

	objManager->update(elapsedTime);
	cameraController->debugUpdate(elapsedTime);
}

void  ModelScene::Gui()
{
	ImGui::Begin("transform");
	ImGui::SliderFloat3("pos", &position.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("scale", &scale.x, 0.01f, 1.0f);
	ImGui::SliderFloat4("rotate", &rotation.x, -10.0f, 10.0f);

	ImGui::SliderInt("index", &animationIndex, 0, 10);
	if (ImGui::Button("animation play"))
	{
		animation->playAnimation(animationIndex, true);
	}
	ImGui::End();
}

void ModelScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };//RGBA(0.0f~1.0f)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	DirectX::XMFLOAT4 cameraPos;
	cameraPos.x = camera->getEye()->x;
	cameraPos.y = camera->getEye()->y;
	cameraPos.z = camera->getEye()->z;
	cameraPos.w = 0.0f;
	objManager->render(*view, *proj, cameraPos);

	Gui();
}