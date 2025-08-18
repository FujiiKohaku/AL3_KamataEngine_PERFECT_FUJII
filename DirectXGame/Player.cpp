#define NOMINMAX
#include "Player.h"
#include <cassert>

Player::Player() {}
Player::~Player() {}

//--------------------------------------------------
// 初期化処理
//--------------------------------------------------
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期向き：右を向かせる
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

//--------------------------------------------------
// 毎フレーム更新処理
//--------------------------------------------------
void Player::Update() {
	// 入力による移動処理
	InputMove();

	// 移動反映
	worldTransform_.translation_ += velocity_;

	// 衝突情報の準備
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	// 地面との接触判定
	if (worldTransform_.translation_.y <= 1.0f) {
		worldTransform_.translation_.y = 1.0f;
		velocity_.y = 0.0f;
		onGround_ = true; // 常に地面にくっつける
	}

	//----------------------------------------
	// 旋回制御（左右の向きをスムーズに補間）
	//----------------------------------------
	if (turnTimer_ > 0.0f) {
		turnTimer_ = (turnTimer_ > 0.0f) ? (turnTimer_ - 1.0f / 60.0f) : 0.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> * 3.0f / 2.0f, // 左向き
		    std::numbers::pi_v<float> / 2.0f,        // 右向き
		};

		float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTrun);
	}

	// 天井との衝突チェック
	CheckMapCollisionUp(collisionMapInfo);

	// 移動量を反映
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井にぶつかったら上方向の速度をリセット
	if (collisionMapInfo.isHitCeiling) {
		velocity_.y = 0;
	}

	// ワールド変換をGPUへ転送
	WorldTransformUpdate(worldTransform_);
}

//--------------------------------------------------
// 描画処理
//--------------------------------------------------
void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

#pragma region 移動処理とジャンプ処理
//--------------------------------------------------
// 入力による移動・ジャンプ制御
//--------------------------------------------------
void Player::InputMove() {

	//----------------------------------------
	// 地上での処理
	//----------------------------------------
	if (onGround_) {
		// 左右移動
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {

			Vector3 acceleration = {};

			// →キー（右）
			if (Input::GetInstance()->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAtteenuation);
				}
				acceleration.x = kAcceleration;

				// 向きを変更する場合
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
			}

			// ←キー（左）
			if (Input::GetInstance()->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAtteenuation);
				}
				acceleration.x = -kAcceleration;

				// 向きを変更する場合
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
			}

			// 加速度を速度に反映
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kMaxSpeed, kMaxSpeed);
		}

		// ジャンプ処理（スペースキー）
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_.y = kjumpAcceleration; // ジャンプ初速
			onGround_ = false;               // 空中状態へ
		}

		// 地上摩擦
		velocity_.x *= (1.0f - kAtteenuation);
	}

	//----------------------------------------
	// 空中処理（重力・落下制限）
	//----------------------------------------
	velocity_.y -= kGravityAcceleration; // 重力を常に加算
	if (velocity_.y < -kLimitFallSpeed) {
		velocity_.y = -kLimitFallSpeed; // 落下速度に下限を設定
	}
}
#pragma endregion

#pragma region 角の位置取得関数
//--------------------------------------------------
// プレイヤー矩形の角座標を取得
//--------------------------------------------------
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorners] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // 右下
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // 左下
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // 右上
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // 左上
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}
#pragma endregion

//--------------------------------------------------
// 天井との衝突判定
//--------------------------------------------------
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上方向に動いていなければ判定不要
	if (info.move.y <= 0) {
		return;
	}

	// プレイヤー矩形の角位置を算出
	std::array<Vector3, kNumCorners> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 左上・右上のマップチップを判定
	bool hit = false;
	MapChipType mapChipType;
	MapChipField::IndexSet indexSet;

	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 天井に当たった場合
	if (hit) {
		// めり込みを防ぐように移動量を修正
		indexSet = mapChipField_->GetMapChipIndexSetByposition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));

		// 天井ヒットを記録
		info.isHitCeiling = true;
	}
}
