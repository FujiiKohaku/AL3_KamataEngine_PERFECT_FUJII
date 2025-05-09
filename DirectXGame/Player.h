#pragma once
#include "Function.h"
#include "KamataEngine.h"
class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();
	// コンストラクタ
	Player();
	// デストラクタ
	~Player();

private: // C++ではメンバ変数は特別な理由がなければprivateにする
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
};
