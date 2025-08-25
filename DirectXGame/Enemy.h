#pragma once
#include "KamataEngine.h"
#include "Math.h"

class Player;

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void UpDate();
	void Draw();

	AABB GetAABB();
	Vector3 GetWorldPosition();

	void OnCollision(const Player* player);

	bool IsDead() const { return isDead_; } // デスフラグGetter

	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	// 状態
	enum class Behavior {
		kNormal,   // 通常
		kDefeated, // 倒された
	};

	Behavior behavior_ = Behavior::kNormal;

	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// 移動用
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kWalkSpeed = 0.01f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 死亡関連
	bool isDead_ = false;
	int deathTimer_ = 0; // 倒された演出用タイマー
	bool isCollisionDisabled_ = false;
};
