#pragma once
#include "KamataEngine.h"
class Player {
public:
	Player();
	~Player();
	// 初期化
	void initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera);
	// 更新
	void UpDate();
	// 描画
	void Draw();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	KamataEngine::Camera* camera_ = nullptr; // カメラへのポインタ

};
