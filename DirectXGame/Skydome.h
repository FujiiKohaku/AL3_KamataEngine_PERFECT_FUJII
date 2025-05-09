#pragma once
#include "KamataEngine.h"
#include "Skydome.h"

// 天球02_02
class Skydome {
public:
	// 初期化関数02_02
	void Initialize();

	// 更新02_02
	void Update();

	// 描画02_02
	void Draw();

private: // 変数はprivate:
	     //  ワールド変換データ02_02
	KamataEngine::WorldTransform worldTransform_;
	// モデル02_02
	KamataEngine::Model* modelSkydome_ = nullptr;
};
