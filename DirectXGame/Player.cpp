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

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	// マップ衝突量チェック

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
	CheckMapCollisionUp(collisionMapInfo);
	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;

	if (collisionMapInfo.isHitCeiling) {
		velocity_.y = 0;
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

#pragma region 角の位置取得関数

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorners] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
#pragma endregion

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorners> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}


	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上の判定
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByposition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先のブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));// これ自分で考えた神やんえらすぎ今日もパーフェクトヒューマン
		// 天井に当たったことを記録
		info.isHitCeiling = true;
	}
}