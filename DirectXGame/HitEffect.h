#pragma once
#include "KamataEngine.h"
class HitEffect {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	// モデル(借りてくる用)
	static KamataEngine::Model* model_;
	// カメラ(借りてくる用)
	static KamataEngine::Camera* camera_;
};
