#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	GameScene();
	~GameScene();
	// シーンが終了したかのゲッター
	bool Finished() const { return finished_; }

private:
	//============
	// 状態管理
	//============
	bool finished_ = false;
	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_;
	// 自キャラ
	Player* player_ = nullptr;

	KamataEngine::Vector3 pos_;

	//---------------
	// skydome
	//---------------
	Skydome* skydome_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;
};