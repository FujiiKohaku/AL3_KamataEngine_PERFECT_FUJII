#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <numbers>

class Player {
public:
	Player();
	~Player();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 position);
	void Update();
	void Draw();

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0, 0, 0};

	// 定数
	const float kMoveSpeed = 0.1f; // 移動速度
	const float kJumpPower = 0.4f; // ジャンプ力
	const float kGravity = 0.02f;  // 重力加速度
	const float kGroundY = 0.0f;   // 地面の高さ

	bool onGround_ = true;    // 接地フラグ
	bool facingRight_ = true; // 向き（右向き:true / 左向き:false）
};
