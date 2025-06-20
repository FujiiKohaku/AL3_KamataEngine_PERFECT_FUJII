#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
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
	//////////////////
	// 関数AL3_02_02//
	///////////////////
	GameScene();
	~GameScene();

	void GenerateBlocks();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera camera_;
	// 自キャラ
	Player* player_ = nullptr;
	// 3Dモデルデータ(block)AL3_02_02
	KamataEngine::Model* modelBlock = nullptr;
	// ブロック用のワールドトランスフォームAL3_02_02
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// デバックカメラ有効AL3_02_02
	bool isDebugCameraActive = false;
	// デバッグカメラAL3_02_02
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	// スカイドームモデル
	KamataEngine::Model* modelSkydome_ = nullptr;
	// スカイドーム
	Skydome* skydome_ = nullptr;
	// マップチップフィールド
	MapChipField* mapChipField_;
	// カメラコントローラ
	CameraController* cController_ = nullptr;

	// 02_09 10枚目 エネミークラス
	Enemy* enemy_ = nullptr;
	// 02_09 10枚目 エネミーモデル
	KamataEngine::Model* enemy_model_ = nullptr;
};