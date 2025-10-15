#pragma once
#include "KamataEngine.h"
#include "Player.h"
// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// シーンが終了したかのゲッター
	bool Finished() const { return finished_; }

private:
	//============
	// 状態管理
	//============
	bool finished_ = false;
	Player* player_;
	// プレイヤーモデル
	KamataEngine::Model* playerModel_ = nullptr;
	// カメラ
	KamataEngine::Camera camera_;
};