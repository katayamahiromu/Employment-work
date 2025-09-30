#include"TimeLapse.h"
#include"imgui.h"
#include"math/Mathf.h"
#include"Animation.h"
#include<algorithm>
#include"system/TimeLapseManager.h"

TimeLapse::TimeLapse()
{
	TimeLapseManager::instance().regist(this);
}

TimeLapse::~TimeLapse()
{

}

void TimeLapse::prepare()
{
	animation = getObject()->GetComponent<Animation>();
}

void TimeLapse::update(float elapsedTime)
{
	isRecord = TimeLapseManager::instance().getIsRecord() && !recordEmpty() ? true : false;
	if (!isRecord)setRecordInfo();
}

void TimeLapse::OnGUI()
{
	const char* text = isRecord ? "Record mode" : "Rewind mode";
	ImGui::Text(text);
	float capacity = static_cast<float>(recordArray.available()) / recordMax * 100;
	ImGui::InputFloat("use buffer", &capacity);
}

void TimeLapse::outputRecordInfo()
{
	recodeInfo receive = getRecord();
	DirectX::XMFLOAT4X4 transform = receive.Transform;
	//受け取った値の内何かがNULLだと無効な値なのでリターン
	if (transform._11 == NULL)return;

	DirectX::XMFLOAT3 p, s;
	DirectX::XMFLOAT4 r;
	Mathf::transformDecomposition(transform, p, s, r);

	Object* o = getObject().get();

	o->setPosition(p);
	o->setScale(s);
	o->setRotation(r);

	animation->setAnimationIndex(receive.animationIndex);
	animation->setAnimationTick(receive.animationTick);
}

void TimeLapse::setRecordInfo()
{
	recodeInfo info;
	info.animationIndex = animation->getAnimationIndex();
	info.animationTick = animation->getAnimationTick();
	info.Transform = *getObject()->getTransform();
	recordArray.pushFront(info);
}