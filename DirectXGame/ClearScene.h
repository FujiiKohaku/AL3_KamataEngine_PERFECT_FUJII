#pragma once
#include "KamataEngine.h"

class ClearScene {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// シーンが終了したかどうか
	bool Finished() const { return finished_; }

private:
	// 状態管理
	bool finished_ = false;

	// モデルとカメラ関連
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
};
