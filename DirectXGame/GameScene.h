#pragma once
#include "CameraController.h"
#include "Goal.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>
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

	void GenerateBlocks();

private:
	//============
	// 状態管理
	//============
	bool finished_ = false;

	//============
	// player
	//============
	KamataEngine::Model* model_ = nullptr; // 3Dモデルデータ
	KamataEngine::Camera* camera_;         // カメラ
	Player* player_ = nullptr;             // 自キャラ

	KamataEngine::Vector3 pos_;

	//---------------
	// skydome
	//---------------
	Skydome* skydome_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	//---------------
	// debugCamera
	//---------------
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	//---------------
	// block
	//---------------

	KamataEngine::Model* modelBlock_;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // 二次元配列

	//---------------
	// MapChipField
	//---------------

	// マップチップフィールド
	MapChipField* mapChipField_;

	//---------------
	// カメラコントローラー
	//---------------

	// カメラコントローラー
	CameraController* cController_;
	//---------------
	// ゴール
	//---------------

	Goal* goal_ = nullptr;
};