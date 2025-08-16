#define NOMINMAX
#include "Player.h"
#include <cassert>

Player::Player() {}

Player::~Player() {}

// 初期化
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールド変換初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期向きを設定（右向き）
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	InputMove();

	// 移動反映
	worldTransform_.translation_ += velocity_;

	// 着地判定
	if (worldTransform_.translation_.y <= 1.0f) {
		worldTransform_.translation_.y = 1.0f;
		velocity_.y = 0.0f;
		onGround_ = true; // 必ず地面にくっつける
	}

	//========================================
	// 旋回制御
	//========================================
	if (turnTimer_ > 0.0f) {
		turnTimer_ = (turnTimer_ > 0.0f) ? (turnTimer_ - 1.0f / 60.0f) : 0.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> * 3.0f / 2.0f, // 左向き
		    std::numbers::pi_v<float> / 2.0f,        // 右向き
		};

		float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTrun);
	}

	//========================================
	// ワールド変換更新
	//========================================
	WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}

#pragma region 移動処理とジャンプ処理
void Player::InputMove() {

	//========================================
	// 移動入力（左右）
	//========================================
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {

			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAtteenuation);
				}
				acceleration.x = kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
			}

			if (Input::GetInstance()->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAtteenuation);
				}
				acceleration.x = -kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
			}

			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kMaxSpeed, kMaxSpeed);
		}

		// ===== ジャンプ処理 =====
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_.y = kjumpAcceleration; // 初速を直接セット
			onGround_ = false;               // 空中へ移行！
		}

		// 地上摩擦
		velocity_.x *= (1.0f - kAtteenuation);
	}

	//========================================
	// 空中処理（重力・着地判定）
	//========================================

	// 重力は常に働かせる
	velocity_.y -= kGravityAcceleration;
	if (velocity_.y < -kLimitFallSpeed) {
		velocity_.y = -kLimitFallSpeed;
	}
}
#pragma endregion