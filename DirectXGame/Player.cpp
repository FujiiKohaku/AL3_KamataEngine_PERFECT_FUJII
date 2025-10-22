#include "Player.h"
#include <cassert>
#include <numbers>
using namespace KamataEngine;

// 初期化
void Player::Initialize(Model* model, Camera* camera, const Vector3 position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

// 更新
void Player::Update() {

	// ===== 入力処理 =====
	if (Input::GetInstance()->PushKey(DIK_D)) { // →キー
		worldTransform_.translation_.x += kMoveSpeed;
		facingRight_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) { // ←キー
		worldTransform_.translation_.x -= kMoveSpeed;
		facingRight_ = false;
	}

	// ===== ジャンプ処理 =====
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onGround_) {
		velocity_.y = kJumpPower;
		onGround_ = false;
	}

	// ===== 重力 =====
	velocity_.y -= kGravity;
	worldTransform_.translation_.y += velocity_.y;

	// ===== 地面との判定 =====
	if (worldTransform_.translation_.y <= kGroundY) {
		worldTransform_.translation_.y = kGroundY;
		velocity_.y = 0;
		onGround_ = true;
	}

	// ===== 向き変更 =====
	if (facingRight_) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	// ===== 行列更新 =====
	WorldTransformUpdate(worldTransform_);
	// --- ImGui デバッグ表示 ---
	ImGui::Begin("Player Debug");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::Text("Rotation Y: %.2f", worldTransform_.rotation_.y);
	ImGui::End();
}

// 描画
void Player::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

// コンストラクタ
Player::Player() {}

// デストラクタ
Player::~Player() {}
