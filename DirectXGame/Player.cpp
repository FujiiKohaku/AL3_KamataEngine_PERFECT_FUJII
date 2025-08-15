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

	//========================================
	// 移動入力
	//========================================
	if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {

		// 加速度ベクトル（毎フレーム初期化）
		Vector3 acceleration = {};

		//------------------------------
		// Dキー（右移動）
		//------------------------------
		if (Input::GetInstance()->PushKey(DIK_D)) {

			// 左移動中に右入力 → 減速させる
			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAtteenuation);
			}

			acceleration.x = kAcceleration;

			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
			}
		}

		//------------------------------
		// Aキー（左移動）
		//------------------------------
		if (Input::GetInstance()->PushKey(DIK_A)) {

			// 右移動中に左入力 → 減速させる
			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - kAtteenuation);
			}

			acceleration.x = -kAcceleration;
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
			}
		}

		// 加速度を速度に反映
		velocity_ += acceleration;

		// 最大速度制限
		velocity_.x = std::clamp(velocity_.x, -kMaxSpeed, kMaxSpeed);

	} else {
		// 非入力時は移動減衰（摩擦）
		velocity_.x *= (1.0f - kAtteenuation);
	}
	// 旋回制御
	{
		// 左右の自キャラの角度テーブル
		float destinationRotationYTable[] = {

		    std::numbers::pi_v<float> * 3.0f / 2.0f, // 左向き
		    std::numbers::pi_v<float> / 2.0f,        // 右向き

		};

		// 状態に応じて向きを変える

		float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection_)];
		//自キャラの角度を設定する
		worldTransform_.rotation_.y = destinationRotationY;

	}

	//========================================
	// 移動反映
	//========================================
	worldTransform_.translation_ += velocity_;

	//========================================
	// ワールド変換更新
	//========================================
	WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}
