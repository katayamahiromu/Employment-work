#pragma once
#define CAST_INT(value)static_cast<int>(value)

//ŠÖ”‚ð’¼Ú“n‚·cˆø”‚È‚µ
#define FUNC_SET_0(func)[this](){this->func();}

//ŠÖ”‚ð’¼Ú“n‚·cˆø”float
#define FUNC_SET_FLOAT(func)[this](float x) { this->func(x); }