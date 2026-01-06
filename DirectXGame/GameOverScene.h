#pragma once
#include "KamataEngine.h"
#include "Skydome.h"
// ゲームオーバーシーン
class GameOverScene {
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

	// モデルとカメラ
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	//---------------
	// skydome
	//---------------
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	uint32_t BgmHandle_ = 0;

	
	Sprite* deadSprite_ = nullptr;
};
