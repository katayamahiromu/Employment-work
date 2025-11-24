#pragma once

//単一のモード
struct ModalMode
{
	float frequency; //周波数
	float amplitude; //振幅
	float decayTime; //減衰時間
	float phase; //初期位相
	float startSec; //開始時刻
	float gain; //モード固有のゲイン
};

extern ModalMode stoneModes[];
extern ModalMode leatherShoeModes[];
extern ModalMode caveRockModes[];