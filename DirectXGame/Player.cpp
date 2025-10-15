#include "Player.h"
#include <cassert>
using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// --- 入力処理 ---
	if (Input::GetInstance()->PushKey(DIK_D)) {
		worldTransform_.translation_.x += kMoveSpeed;
		facingRight_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		worldTransform_.translation_.x -= kMoveSpeed;
		facingRight_ = false;
	}

	// --- ジャンプ ---
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onGround_) {
		velocity_.y = kJumpPower;
		onGround_ = false;
	}

	// --- 重力 ---
	velocity_.y -= kGravity;
	worldTransform_.translation_.y += velocity_.y;

	// --- 地面との接地判定 ---
	if (worldTransform_.translation_.y <= kGroundHeight) {
		worldTransform_.translation_.y = kGroundHeight;
		velocity_.y = 0;
		onGround_ = true;
	}

	// --- 向き変更（Y軸回転）---
	worldTransform_.rotation_.y = facingRight_ ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;


	

	// --- 行列更新 ---
	worldTransform_.TransferMatrix(worldTransform_);


}

void Player::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

KamataEngine::Vector3 Player::GetWorldPosition() const {
	KamataEngine::Vector3 pos;
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}