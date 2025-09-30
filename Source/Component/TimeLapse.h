#pragma once
#include"../Utils/RingBuffer.h"
#include"Component/Component.h"
#include"Component/Animation.h"

struct recodeInfo
{
	//�A�j���[�V�����ԍ�
	int animationIndex;
	//�A�j���[�V�����Đ�����
	float animationTick;
	DirectX::XMFLOAT4X4 Transform;
	//int actionType;
};

class TimeLapse : public Component
{
public:
	TimeLapse();
	~TimeLapse();

	// ���O�擾
	const char* getName() const override { return "TimeLapse"; }

	// �J�n����
	void prepare()override;

	// �X�V����
	void update(float elapsedTime);

	// GUI�`��
	void OnGUI();

	//�L�^
	void setRecordInfo();

	//�L�^�̎��o��
	void outputRecordInfo();

	//�o�b�t�@�[�̎g�p��
	float occupancyRate(){return static_cast<float>(recordArray.available()) / recordMax * 100;}
private:
	//��납�珇�ɃA�N�Z�X����
	recodeInfo getRecord() { return recordArray.frontAndPop(); }

	bool recordEmpty() { return recordArray.isEmpty(); }
private:
	static constexpr int recordMax = 144 * 3;
	int subscript = 0;
	RingBuffer<recodeInfo, recordMax>recordArray;
	std::shared_ptr<Animation>animation;
	bool isRecord = true;
};