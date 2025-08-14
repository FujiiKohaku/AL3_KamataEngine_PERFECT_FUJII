#pragma once

#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "Player.h"
#include "Skydome.h"
#include <list>
#include <vector>

// ==========================
// ゲームシーン クラス
// ==========================
class GameScene {
public:
	GameScene();
	~GameScene();

	// 初期化（リソース読み込みやオブジェクト生成）
	void Initialize();

	// 毎フレーム更新
	void Update();

	// 毎フレーム描画
	void Draw();

	// マップチップなどからブロックを生成
	void GenerateBlocks();

	// 全てのオブジェクト間の衝突判定と応答処理
	void CheckAllCollisions();

	// シーン終了フラグ取得
	bool IsFinished() const { return finished_; }

	// ヒットエフェクトを指定位置に生成
	void CreateHitEffect(const KamataEngine::Vector3& position);

private:
	// ===== ゲームの進行状態 =====
	enum class Phase {
		kFadeIn, // 開始フェード中
		kPlay,   // プレイ中
		kDeath,  // デス演出中
		kFadeOut // 終了フェード中
	};
	Phase phase_; // 現在のフェーズ

	// フェーズ切り替え処理
	void ChangePhase();

	// ===== リソース / モデル =====
	uint32_t textureHandle_ = 0;                 // 汎用テクスチャハンドル
	KamataEngine::Model* model_ = nullptr;       // プレイヤーなどのメインモデル
	KamataEngine::Model* modelAttack_ = nullptr; // 攻撃用モデル
	KamataEngine::Model* modelBlock = nullptr;   // ブロック用モデル

	// ===== ブロック配置 =====
	// 二次元配列形式でWorldTransformのポインタを保持
	// [行][列]でアクセス、生成はGenerateBlocksで行い、破棄時はdeleteとclear
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// ===== カメラ関連 =====
	KamataEngine::Camera camera_;                      // メインカメラ
	bool isDebugCameraActive = false;                  // デバッグカメラ有効フラグ
	KamataEngine::DebugCamera* debugCamera_ = nullptr; // デバッグカメラ

	// ===== 環境オブジェクト =====
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドームモデル
	Skydome* skydome_ = nullptr;                  // スカイドーム本体
	MapChipField* mapChipField_ = nullptr;        // マップチップ管理
	CameraController* cController_ = nullptr;     // カメラ操作コントローラ

	// ===== プレイヤー関連 =====
	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayerHit_ = nullptr; // プレイヤー被弾時モデル

	// ===== エネミー関連 =====
	KamataEngine::Model* enemy_model_ = nullptr; // 敵モデル
	std::list<Enemy*> enemies_;                  // 敵リスト

	// ===== エフェクト関連 =====
	KamataEngine::Model* modelParticle_ = nullptr;     // 汎用パーティクルモデル
	std::list<HitEffect*> hitEffects_;                 // ヒットエフェクトリスト
	DeathParticles* deathParticles_ = nullptr;         // デス演出パーティクル
	KamataEngine::Model* deathParticleModel = nullptr; // デスパーティクルモデル

	// ===== その他 =====
	GameScene* gameScene = nullptr; // 自己参照？用途不明（不要なら削除推奨）
	bool finished_ = false;         // シーン終了フラグ
	Fade* fade_ = nullptr;          // フェード管理
};
