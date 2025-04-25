#pragma once
#include "KamataEngine.h"
#include "Player.h"

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

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_;
	// 自キャラ
	Player* player_ = nullptr;
	// 3Dモデルデータ(block)AL3_02_02
	KamataEngine::Model* modelBlock = nullptr;
	// ブロック用のワールドトランスフォームAL3_02_02
	std::vector<KamataEngine::WorldTransform*> worldTransformBlocks_;
};