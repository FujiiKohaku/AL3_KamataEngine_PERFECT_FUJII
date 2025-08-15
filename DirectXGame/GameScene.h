#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>

// ゲームシーン
class GameScene {
public:
	~GameScene();

	// ライフサイクル
	void Initialize();
	void Update();
	void Draw();

private:
	// ===== リソース =====
	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* skydomeModel_ = nullptr;

	// テクスチャ
	uint32_t textureHandle_ = 0;

	// ===== 変換・カメラ =====
	KamataEngine::WorldTransform worldtransform_;
	KamataEngine::WorldTransform worldTransformSkydome_; // ← 名前空間を明示
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// ===== ゲームオブジェクト =====
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;

	// ブロック用ワールドトランスフォーム（インスタンスごとに所有）
	std::vector<KamataEngine::WorldTransform*> worldTransformBlocks_;

	// ===== フラグ =====
	bool isDebugCameraActive_ = false;

	//===== マップチップフィールド =====
	MapChipField* mapChipField_; // マップチップフィールド

};
