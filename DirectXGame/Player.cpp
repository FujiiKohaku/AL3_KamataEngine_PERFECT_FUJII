#define NOMINMAX
#include "Player.h"
#include "WorldRowFunction.h"
#include "imgui.h"
#include <cassert>

using namespace KamataEngine;

// 初期化
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// 引数の内容を記録
	camera_ = camera;

	// テクスチャハンドル

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// bullets_.resize(kMaxBullets);
}

// 更新
void Player::Update() {

	MoveInput(); // 移動処理

	JumpInput();      // ジャンプ処理
	UpDateRotation(); // 向きの制御

	Shoot();
	// 位置に加算（移動）
	worldTransform_.translation_ += velocity_;

	WorldRowFunction::MakeAffinTransFerMatrix(worldTransform_);

	// 弾発射処理
	// 弾の更新
	// for (auto& bullet : bullets_) {
	//	bullet.Update();
	//}

	// ===== ImGui デバッグ表示 =====
	ImGui::Begin("Player Debug");
	ImGui::Text("Pos: (%.2f, %.2f, %.2f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::Text("Vel: (%.2f, %.2f, %.2f)", velocity_.x, velocity_.y, velocity_.z);
	ImGui::End();
}

// 描画
void Player::Draw() {
	model_->Draw(worldTransform_, *camera_);

	//// 弾の描画
	// for (auto& bullet : bullets_) {
	//	bullet.Draw(camera_);
	// }
}

// コンストラクタ
Player::Player() {}

// デストラクタ
Player::~Player() {}

#pragma region Moveinput

void Player::MoveInput() {
	Vector3 move = {0, 0, 0};

	if (Input::GetInstance()->PushKey(DIK_D)) { // →右
		move.x += 1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) { // ←左
		move.x -= 1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_W)) { // ↑上
		move.y += 1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) { // ↓下
		move.y -= 1.0f;
	}

	// 斜め入力を正規化してスピード一定にする
	if (move.x != 0.0f || move.y != 0.0f) {
		float length = std::sqrt(move.x * move.x + move.y * move.y);
		move.x /= length;
		move.y /= length;
	}

	velocity_ = move * moveSpeed; // 入力方向 × 移動速度
}

#pragma endregion

#pragma region jumpInput

void Player::JumpInput() {

	// ジャンプ
	if (Input::GetInstance()->PushKey(DIK_UP)) {
		// ジャンプ初速
		velocity_ += Vector3(0, kJumpAcceleration / 60, 0);
	}
}

#pragma endregion

#pragma region UpDateRotation
void Player::UpDateRotation() {
	// 最大傾き角度（ラジアン）
	const float tiltMax = 0.3f; // 約17度

	// 目標の傾き
	float targetTilt = 0.0f;

	if (Input::GetInstance()->PushKey(DIK_A)) {
		targetTilt = tiltMax; // 左に傾く
	} else if (Input::GetInstance()->PushKey(DIK_D)) {
		targetTilt = -tiltMax; // 右に傾く
	}

	// スムーズに補間して傾きを更新
	worldTransform_.rotation_.x = Lerp(worldTransform_.rotation_.z, targetTilt, 0.2f);
}
#pragma endregion


