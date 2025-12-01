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

	float bandwidth;     // 共振の幅（倍音を削る効果）
	float inharmonicity; // 倍音のずれ（0なら完全整数比）
	float noiseMix;      // ノイズ成分の割合
	float harmonicMask;  // 倍音削り用の重み（0〜1）

	float randomPhase;   // 位相のランダム揺らぎ量（0〜0.1程度）
	float randomDecay;   // 減衰時間のランダム揺らぎ率（例: 0.1 → ±10%）
	float clipAmount;    // 非線形クリッピング強度（0なら無効、0.2〜0.5程度）
	float lowpassCutoff; // ローパスフィルタのカットオフ周波数（Hz）
	float highpassCutoff;// ハイパスフィルタのカットオフ周波数（Hz）
};

extern ModalMode stoneModes[];
extern ModalMode leatherShoeModes[];
extern ModalMode caveRockModes[];