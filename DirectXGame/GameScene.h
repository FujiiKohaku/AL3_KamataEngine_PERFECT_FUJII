#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Enemy2.h"
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "Player.h"
#include "Skydome.h"
#include <list>
#include <vector>

// ゲームシーン
class GameScene {
public:
	~GameScene();

	// ライフサイクル
	void Initialize();
	void Update();
	void Draw();

	void GenerateBlocks();
	void CheckAllCollisions();

	bool IsFinished() const { return finished_; }
	bool IsClear() const { return isClear_; }

private:
	// ===== リソース =====
	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelRolling = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* skydomeModel_ = nullptr;
	KamataEngine::Model* enemyModel_ = nullptr;
	KamataEngine::Model* dethParticleModel = nullptr;

	// Ready / Go!! モデル
	KamataEngine::Model* readyModel_ = nullptr;
	KamataEngine::Model* goModel_ = nullptr;

	// テクスチャ
	uint32_t textureHandle_ = 0;

	// ===== 変換・カメラ =====
	KamataEngine::WorldTransform worldtransform_;
	KamataEngine::WorldTransform worldTransformSkydome_;
	KamataEngine::WorldTransform worldTransformReady_;
	KamataEngine::WorldTransform worldTransformGo_;

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// ===== ゲームオブジェクト =====
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;
	std::list<Enemy*> enemies_;
	std::list<Enemy2*> enemies2_;

	// ブロック用ワールドトランスフォーム（インスタンスごとに所有）
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// ===== フラグ =====
	bool isDebugCameraActive_ = false;

	// ===== マップチップフィールド =====
	MapChipField* mapChipField_ = nullptr;

	// ===== カメラコントローラー =====
	CameraController* cController_ = nullptr;

	// ===== デス演出 =====
	DeathParticles* deathParticles_ = nullptr;

	// ===== フェーズ管理 =====
	enum class Phase {
		kFadeIn,
		kReady, // READY-GO!! 演出
		kPlay,
		kDeath,
		kFadeOut,
	};
	Phase phase_ = Phase::kFadeIn;

	float readyTimer_ = 0.0f; // READY-GO!! 用タイマー

	void ChangePhese();
	bool finished_ = false;
	Fade* fade_ = nullptr;

	std::list<HitEffect*> hitEffects_;

	Model* spikeModel_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformSpikes_;

	std::vector<std::vector<WorldTransform*>> worldTransformSprings_;
	std::vector<std::vector<WorldTransform*>> worldTransformFires_;
	WorldTransform* worldTransformGoal_ = nullptr;
	KamataEngine::Model* goalModel_ = nullptr;        
	Model* fireModel_ = nullptr;

	Model* springModel_ = nullptr;

	Model* enemyModel2_;
	bool isClear_ = false;
};
