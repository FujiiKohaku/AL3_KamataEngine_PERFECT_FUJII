#define NOMINMAX
#include "Player.h"
#include "WorldRowFunction.h"
#include "imgui.h"
#include <cassert>

using namespace KamataEngine;

//======================================================================
// 初期化
//======================================================================
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	modelAttack_ = modelAttack;
	camera_ = camera;

	// 自機
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// 攻撃モデル
	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = position;
	worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

#pragma region 移動処理関数
//======================================================================
// 移動処理（左右入力／ジャンプ・重力） AL3_02_07 p10
//======================================================================
void Player::InputMove() {
	// 接地中の左右移動
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 逆向き入力は急減速
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				// 向き変更
				if (lrDirection_ != LRDorection::kRight) {
					lrDirection_ = LRDorection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
				acceleration.x += kAccelaration_;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDorection::kLeft) {
					lrDirection_ = LRDorection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.1f;
				}
				acceleration.x -= kAccelaration_;
			}

			// 速度反映＆上限
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は減衰
			velocity_.x *= (1.0f - kAccelaration_);
		}

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ += Vector3(0, kJumpAcceleration / 60, 0);
		}
	}
	// 空中：重力・落下制限
	else {
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}
#pragma endregion

#pragma region マップ衝突判定 AL3_02_07
//======================================================================
// マップ衝突 判定まとめ（上下左右） AL02_03 p14
//======================================================================
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

//======================================================================
// 上方向 当たり判定 AL3_02_07 p19
//======================================================================
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 移動後の四隅座標
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	// 上昇なしなら不要
	if (info.move.y <= 0) {
		return;
	}

	MapChipType mapChipType, mapChipTypeNext;
	bool hit = false;

	// 左上
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ヒット時：天井で押し戻し
	Vector3 playerTopPos = worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0);
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(playerTopPos);
		(void)indexSetNow; // 現在マス参照（処理は元のまま）

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.isHitCeiling = true;
	}
}

//======================================================================
// 下方向 当たり判定 02_08
//======================================================================
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	// 下降なしなら不要
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の四隅座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType, mapChipTypeNext;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ヒット時：地面で押し上げ
	Vector3 playerBottomPos = worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0);
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(playerBottomPos);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			info.isHitLanding = true;
		}
	}
}

//======================================================================
// 右方向 当たり判定 02_08 p25
//======================================================================
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0)
		return;

	// 移動後の四隅
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;

	// 右側のマスをチェック（右上／右下）
	for (Corner corner : {kRightTop, kRightBottom}) {
		auto index = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[corner]);
		auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (mapChip == MapChipType::kBlock) {
			hit = true;
			break;
		}
	}

	if (hit) {
		// マス跨ぎチェック
		Vector3 playerRightPosNow = worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0);
		Vector3 playerRightPosNext = worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0);

		auto indexNow = mapChipField_->GetMapChipIndexSetByPosition(playerRightPosNow);
		auto indexNext = mapChipField_->GetMapChipIndexSetByPosition(playerRightPosNext);

		if (indexNow.xIndex != indexNext.xIndex) {
			auto rect = mapChipField_->GetRectByIndex(indexNext.xIndex, indexNext.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.isHitWall = true;
		}
	}
}

//======================================================================
// 左方向 当たり判定 02_08 p25
//======================================================================
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;

	// 左上
	{
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (type == MapChipType::kBlock) {
			hit = true;
		}
	}
	// 左下
	{
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (type == MapChipType::kBlock) {
			hit = true;
		}
	}

	if (hit) {
		Vector3 leftPosAfterMove = worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0);
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(leftPosAfterMove);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex);

		// そのマスの右端までに調整（微小隙間を確保）
		float overlap = leftPosAfterMove.x - rect.right;
		if (overlap < 0.0f) {
			info.move.x -= overlap + kBlank;
		}
		info.isHitWall = true;
	}
}
#pragma endregion

//======================================================================
// 接地状態の切り替え 02_08 p14
//======================================================================
void Player::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		// 上昇で空中へ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 足元探索（左右）
			std::array<Vector3, kNumCorner> positionNew;
			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}
			MapChipType mapChipType;
			bool hit = false;
			MapChipField::IndexSet indexSet;

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		// 着地
		if (info.isHitLanding) {
			onGround_ = true;
			velocity_.x *= (1.0f, -kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

//======================================================================
// 四隅座標（center基準） AL3_02_07 p18
//======================================================================
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

//======================================================================
// 更新
//======================================================================
void Player::Update() {
	// ヘイビア切替（初期化）
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	// ヘイビア更新
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	// 行列計算
	WorldTransformUpdate(worldTransform_);
	WorldTransformUpdate(worldTransformAttack_);
}

//======================================================================
// 描画
//======================================================================
void Player::Draw() {
	model_->Draw(worldTransform_, *camera_);

	// 攻撃フェーズに応じて攻撃モデル描画
	switch (attackPhase_) {
	case AttackPhase::kUnknown:
		break;
	case AttackPhase::kAnticipation:
		break;
	case AttackPhase::kAction:
	case AttackPhase::kRecovery:
		modelAttack_->Draw(worldTransformAttack_, *camera_);
		break;
	}
}

//======================================================================
// ワールド座標・AABB
//======================================================================
Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

//======================================================================
// 敵との衝突
//======================================================================
void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	if (IsAttack()) {
		return;
	} // 攻撃中は無敵
	isDead_ = true;
}

//======================================================================
// ヘイビア：通常
//======================================================================
void Player::BehaviorRootUpdate() {
	// 入力→移動
	Player::InputMove();

	// 衝突判定準備
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	// マップ衝突→位置反映
	CheckMapCollision(collisionMapInfo);
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井に当たったら上向き速度をゼロ
	if (collisionMapInfo.isHitCeiling) {
		velocity_.y = 0;
	}

	// 接地状態更新
	UpdateOnGround(collisionMapInfo);

	// 下降中の簡易地面判定（元コード維持）
	bool landing = false;
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}
	(void)landing;

	// 向きの回転補間
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, 1.0f - turnTimer_ / kTimeTrun);
	}

	// 攻撃開始
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	// 行列更新（補助関数）
	WorldRowFunction::MakeAffinTransFerMatrix(worldTransform_);
}

void Player::BehaviorRootInitialize() {}

//======================================================================
// ヘイビア：攻撃
//======================================================================
void Player::BehaviorAttackInitialize() {
	// カウンタ初期化
	attackParameter_ = 0;
	velocity_ = {};
	// 溜めから開始
	attackPhase_ = AttackPhase::kAnticipation;
}

void Player::BehaviorAttackUpdate() {
	// 攻撃移動のベース速度
	const Vector3 attackVelocity = {0.8f, 0.0f, 0.0f};
	Vector3 velocity{};

	// フェーズ時間
	attackParameter_++;

	// フェーズ別処理
	switch (attackPhase_) {
	case AttackPhase::kAnticipation: { // 溜め
		velocity = {};
		float t = static_cast<float>(attackParameter_) / kAnticipationTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);
		if (attackParameter_ >= kAnticipationTime) {
			attackPhase_ = AttackPhase::kAction;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kAction: { // 突進
		velocity = (lrDirection_ == LRDorection::kRight) ? +attackVelocity : -attackVelocity;
		float t = static_cast<float>(attackParameter_) / kActionTime;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);
		if (attackParameter_ >= kActionTime) {
			attackPhase_ = AttackPhase::kRecovery;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kRecovery: { // 余韻
		velocity = {};
		float t = static_cast<float>(attackParameter_) / kRecoveryTime;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);
		if (attackParameter_ >= kRecoveryTime) {
			behaviorRequest_ = Behavior::kRoot;
			attackPhase_ = AttackPhase::kUnknown;
		}
		break;
	}
	default:
		break;
	}

	// 衝突チェック→移動
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity;
	collisionMapInfo.isHitLanding = false;
	collisionMapInfo.isHitWall = false;

	CheckMapCollision(collisionMapInfo);
	worldTransform_.translation_ += collisionMapInfo.move;

	// 方向転換の回転補間
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTrun);
	}

	// 攻撃モデルの追従
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
}

//======================================================================
// コンストラクタ／デストラクタ
//======================================================================
Player::Player() {}
Player::~Player() {}
