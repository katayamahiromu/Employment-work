#pragma once

#include"object.h"

//�R���|�[�l���g
class Component
{
public:
	Component() {};
	virtual ~Component() {};

	// ���O�擾
	virtual const char* getName() const = 0;

	// �J�n����
	virtual void prepare() {}

	// �X�V����
	virtual void update(float elapsedTime) {}

	// GUI�`��
	virtual void OnGUI() {}

	// �A�N�^�[�ݒ�
	void setObject(std::shared_ptr<Object> object) { this->object = object; }

	// �A�N�^�[�擾
	std::shared_ptr<Object> getObject() { return object.lock(); }
private:
	std::weak_ptr<Object>object;
};