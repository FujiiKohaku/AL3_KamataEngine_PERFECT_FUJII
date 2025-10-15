#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <algorithm>
#include <array>
#include <numbers>

//==================================================
// プレイヤークラス
//==================================================
class Player {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// 現在のワールド座標を返す
	KamataEngine::Vector3 GetWorldPosition() const;

private:
	// --- 定数 ---
	const float kGravity = 0.02f;     // 重力加速度
	const float kJumpPower = 0.5f;    // ジャンプ初速
	const float kMoveSpeed = 0.05f;   // 移動速度
	const float kGroundHeight = 0.0f; // 地面Y座標

	// --- 状態 ---
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {0, 0, 0};
	bool onGround_ = false;
	bool facingRight_ = true;
};