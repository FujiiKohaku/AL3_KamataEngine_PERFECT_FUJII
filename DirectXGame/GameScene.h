#pragma once
#include "KamataEngine.h"

// ゲームシーン
class GameScene {

public:          // 関数はpublicより↓
	GameScene(); // コンストラクタ

	~GameScene(); // デストラクタ

	// 関数定義//
	//  初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// 変数定義//変数はprivateより↓
private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// サウンドデータハンドル
	uint32_t soundDataHandle_ = 0;
	// 音声再生ハンドル
	uint32_t voiceHandle_ = 0;
	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransForm_;
	// カメラ
	KamataEngine::Camera camera_;
	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

};