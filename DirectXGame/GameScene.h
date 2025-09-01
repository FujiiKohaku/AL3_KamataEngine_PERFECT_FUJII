#pragma once
#include "Bullet.h"
#include "CameraController.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "Player.h"
#include "Skydome.h"
#include <array>
// ゲームシーン
class GameScene {
public:
	//////////////////
	// 関数AL3_02_02//
	///////////////////
	GameScene();
	~GameScene();

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();


	/*void GenerateBlocks();*/

private:
	// ===== モデル関連 =====
	KamataEngine::Model* model_ = nullptr;        // 自キャラ用
	KamataEngine::Model* modelBlock = nullptr;    // ブロック用 AL3_02_02
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドーム用
	KamataEngine::Model* modelBullet_ = nullptr;  // 弾用
	// ===== カメラ関連 =====
	KamataEngine::Camera camera_;                      // メインカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr; // デバッグカメラ AL3_02_02
	bool isDebugCameraActive = false;                  // デバッグカメラ有効フラグ AL3_02_02
	CameraController* cController_ = nullptr;          // カメラコントローラ

	static const uint32_t kMaxBullets = 50; // 最大弾数
	// ===== オブジェクト関連 =====
	Player* player_ = nullptr;   // 自キャラ
	Skydome* skydome_ = nullptr; // スカイドーム

	std::array<Bullet*, kMaxBullets> bullets_; // 複数弾を固定数で管理

	// ===== マップ関連 =====
	// MapChipField* mapChipField_ = nullptr;                                         // マップチップフィールド
	// std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // ブロックのワールドトランスフォーム AL3_02_02

	// ===== その他 =====
	uint32_t textureHandle_ = 0; // テクスチャハンドル
};
