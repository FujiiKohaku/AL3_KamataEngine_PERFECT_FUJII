#define NOMINMAX
#include "Player.h"
#include "imgui.h"
#include <cassert>

using namespace KamataEngine;

//==================================================
// コンストラクタ・デストラクタ
//==================================================
Player::Player() {}
Player::~Player() {}

//==================================================
// 初期化処理
//==================================================
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelRollling, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);

	model_ = model;

	camera_ = camera;

	modelRollling_ = modelRollling;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期向き：右向き
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

#pragma region 入力処理ジャンプ関数化
//==================================================
// 入力処理（移動・ジャンプ）
//==================================================
void Player::InputMove() {
	if (onGround_) {
		// 左右移動
		if (Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {

			Vector3 acceleration = {};

			// 右入力
			if (Input::GetInstance()->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation); // 急ブレーキ
				}
				if (lrDirection_ != LRDorection::kRight) {
					lrDirection_ = LRDorection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
				acceleration.x += kAccelaration_;

				// 左入力
			} else if (Input::GetInstance()->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation); // 急ブレーキ
				}
				if (lrDirection_ != LRDorection::kLeft) {
					lrDirection_ = LRDorection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.1f;
				}
				acceleration.x -= kAccelaration_;
			}

			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 入力なし → 減速
			velocity_.x *= (1.0f - kAccelaration_);
		}

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_W)) {
			velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
		}

	} else {
		// 空中：重力加速
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed); // 落下速度制限
	}
}
#pragma endregion

//==================================================
// 四隅の座標を返す
//==================================================
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // 右下
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // 左下
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // 右上
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // 左上
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}
#pragma region プレイヤーの当たり判定まとめ
//==================================================
// マップ衝突判定の統合呼び出し
//==================================================
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}
#pragma endregion

#pragma region 天井
//--------------------------------------------------
// 天井との衝突判定
//--------------------------------------------------
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0)
		return; // 上昇してなければ不要

	// 四隅の座標
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	MapChipType mapChipType, mapChipTypeNext;
	MapChipField::IndexSet indexSet;

	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock)
		hit = true;

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock)
		hit = true;

	if (hit) {
		Vector3 playerTopPos = worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0);
		indexSet = mapChipField_->GetMapChipIndexSetByposition(playerTopPos);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.isHitCeiling = true;
	}
}
#pragma endregion

#pragma region 床との衝突判定
//--------------------------------------------------
// 床との衝突判定
//--------------------------------------------------
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0)
		return; // 下降してなければ不要

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	MapChipType mapChipType, mapChipTypeNext;
	MapChipField::IndexSet indexSet;

	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock)
		hit = true;

	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock)
		hit = true;

	if (hit) {
		Vector3 playerBottomPos = worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0);
		indexSet = mapChipField_->GetMapChipIndexSetByposition(playerBottomPos);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		info.isHitLanding = true;
	}
}
#pragma endregion

#pragma region 右側の衝突判定
//--------------------------------------------------
// 右側との衝突判定
//--------------------------------------------------
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	for (Corner corner : {kRightTop, kRightBottom}) {
		auto index = mapChipField_->GetMapChipIndexSetByposition(positionsNew[corner]);
		auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (mapChip == MapChipType::kBlock) {
			hit = true;
			break;
		}
	}

	if (hit) {
		Vector3 playerRightPosNext = worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0);
		auto index = mapChipField_->GetMapChipIndexSetByposition(playerRightPosNext);
		auto rect = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex);

		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info.isHitWall = true;
	}
}
#pragma endregion

#pragma region 左側の衝突判定
//--------------------------------------------------
// 左側との衝突判定
//--------------------------------------------------
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	for (Corner corner : {kLeftTop, kLeftBottom}) {
		auto index = mapChipField_->GetMapChipIndexSetByposition(positionsNew[corner]);
		auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (mapChip == MapChipType::kBlock)
			hit = true;
	}

	if (hit) {
		Vector3 leftPosAfterMove = worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0);
		auto index = mapChipField_->GetMapChipIndexSetByposition(leftPosAfterMove);
		auto rect = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex);

		float overlap = leftPosAfterMove.x - rect.right;
		if (overlap < 0.0f) {
			info.move.x -= overlap + kBlank; // 微小な隙間を空ける
		}
		info.isHitWall = true;
	}
}
#pragma endregion

#pragma region 接地状態の更新
//==================================================
// 接地状態の更新
//==================================================
void Player::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false; // ジャンプ開始
		} else {
			// 落下判定
			std::array<Vector3, kNumCorner> positionNew;
			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			bool hit = false;
			for (Corner corner : {kLeftBottom, kRightBottom}) {
				auto index = mapChipField_->GetMapChipIndexSetByposition(positionNew[corner] + Vector3(0, -kGroundSearchHeight, 0));
				auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
				if (mapChip == MapChipType::kBlock)
					hit = true;
			}
			if (!hit)
				onGround_ = false; // 落下開始
		}
	} else {
		if (info.isHitLanding) {
			onGround_ = true; // 着地
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}
#pragma endregion

#pragma region 壁衝突時
//==================================================
// 壁衝突時の処理
// 壁にぶつかったら横方向の速度をリセットする
//==================================================
void Player::UpdateOnWall(const CollisionMapInfo& info) {
	if (info.isHitWall) {
		velocity_.x = 0.0f;
	}
}
#pragma endregion
//==================================================
// 更新処理
//==================================================
void Player::Update() {
	InputMove();

	UpdateState();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 移動反映
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井に当たったら上方向の速度リセット
	if (collisionMapInfo.isHitCeiling) {
		velocity_.y = 0;
	}

	// 接地判定
	UpdateOnGround(collisionMapInfo);
	UpdateOnWall(collisionMapInfo);
	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, 1.0f - turnTimer_ / kTimeTrun);
	}

	WorldTransformUpdate(worldTransform_);
}

//==================================================
// 描画処理
//==================================================
void Player::Draw() {
	if (state_ == PlayerState::Rolling) {
		modelRollling_->Draw(worldTransform_, *camera_);
	} else {
		model_->Draw(worldTransform_, *camera_);
	}
}

//==================================================
// ユーティリティ
//==================================================

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	return {
	    {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f},
        {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f}
    };
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	isDead_ = true;
}

KamataEngine::Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	KamataEngine::Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void Player::UpdateState() {
	// --- 入力で状態遷移 ---
	if (Input::GetInstance()->TriggerKey(DIK_O)) {
		if (state_ == PlayerState::Normal) {
			// 通常 → ローリング
			state_ = PlayerState::Rolling;
		} else {
			// ローリング → 通常に戻る前にチェック
			float nextHeight = 1.5f; // ノーマルの高さ
			Vector3 topPos = worldTransform_.translation_ + Vector3(0, nextHeight / 2.0f, 0);

			auto index = mapChipField_->GetMapChipIndexSetByposition(topPos);
			auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

			if (mapChip != MapChipType::kBlock) {
				state_ = PlayerState::Normal; // 戻せる
			}
		}
	}

	// --- 高さを設定 ---
	if (state_ == PlayerState::Rolling) {
		kHeight = 0.8f;
	} else {
		kHeight = 1.5f;
	}

	// --- 下の判定（地面に埋まらないように補正） ---
	if (state_ == PlayerState::Normal) {
		Vector3 bottomPos = worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0);
		auto index = mapChipField_->GetMapChipIndexSetByposition(bottomPos);
		auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

		if (mapChip == MapChipType::kBlock) {
			auto rect = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex);
			worldTransform_.translation_.y = rect.top + kHeight / 2.0f + kBlank;
		}
	}

	// --- 上の判定（天井にぶつかったら補正） ---
	if (state_ == PlayerState::Normal) {
		Vector3 topPos = worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0);
		auto index = mapChipField_->GetMapChipIndexSetByposition(topPos);
		auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

		if (mapChip == MapChipType::kBlock) {
			auto rect = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex);
			worldTransform_.translation_.y = rect.bottom - kHeight / 2.0f - kBlank;
			velocity_.y = 0.0f; // 天井にぶつかったのでリセット
		}
	}
}
