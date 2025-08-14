#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"
#include <vector>
// ゲームシーン
class GameScene {
public:
	~GameScene();

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	// 3dmodelの読み込み
	KamataEngine::Model* model_ = nullptr;
	// テクスチャ
	uint32_t textureHandle_ = 0;

	KamataEngine::WorldTransform worldtransform_;
	KamataEngine::Camera* camera_;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_;

	Player* player_;

	// 3dモデルの読み込み
	KamataEngine::Model* modelBlock_ = nullptr;

	// ブロック用のワールドトランスフォーム
	std::vector<KamataEngine::WorldTransform*> worldTransformBlocks_;

	// デバッグカメラ有効フラグ
	bool isDebugCameraActive_ = false;
};