#pragma once
#define CAST_INT(value)static_cast<int>(value)

//�֐��𒼐ړn���c�����Ȃ�
#define FUNC_SET_0(func)[this](){this->func();}

//�֐��𒼐ړn���c����float
#define FUNC_SET_FLOAT(func)[this](float x) { this->func(x); }