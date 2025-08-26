#define NOMINMAX
#include "Player.h"
#include "imgui.h"
#include <cassert>
#include "Enemy.h"
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

	dustModel_ = Model::CreateFromOBJ("particle", true);

	dust_.Initialize(dustModel_, camera_);
}

#pragma region 入力処理ジャンプ関数化
//==================================================
// 入力処理（移動・ジャンプ）
//==================================================
void Player::InputMove() {
	// ===== 左右移動 =====
	if (Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {
		Vector3 acceleration = {};

		// → 右移動
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
		}

		// ← 左移動
		if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - kAttenuation); // 急ブレーキ
			}
			if (lrDirection_ != LRDorection::kLeft) {
				lrDirection_ = LRDorection::kLeft;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTrun;
			}
			acceleration.x -= kAccelaration_;
		}

		velocity_ += acceleration;
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	} else {
		// 入力なし → 減速
		velocity_.x *= (1.0f - kAttenuation);
	}

	// ===== ジャンプ処理（二段ジャンプ対応） =====
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (onGround_ || jumpCount_ < kMaxJumpCount) {
			velocity_.y = kJumpInitialVelocity; // 初速を与える
			onGround_ = false;
			jumpCount_++; // ジャンプ回数を増やす
		}
	}

	// ★ ジャンプ中の長押し補助（上昇中だけ）
	if (Input::GetInstance()->PushKey(DIK_SPACE) && velocity_.y > 0) {
		velocity_.y += kJumpBoost / 60.0f;
	}

	// ★ 重力を加える
	velocity_.y -= kGravityAcceleration / 60.0f;
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed); // 落下速度制限

	// ===== 地面に着いたらジャンプ回数リセット =====
	if (onGround_) {
		jumpCount_ = 0;
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
	MapChipField::IndexSet indexSet;

	// 左下と右下をチェック
	for (Corner corner : {kLeftBottom, kRightBottom}) {
		indexSet = mapChipField_->GetMapChipIndexSetByposition(positionsNew[corner]);
		MapChipType mapChip = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

		// ★ トゲなら即死
		if (mapChip == MapChipType::kSpike) {
			isDead_ = true;
			return;
		}

		// ブロック判定
		MapChipType mapChipNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		if (mapChip == MapChipType::kBlock && mapChipNext != MapChipType::kBlock) {
			hit = true;
		}
	}

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
	MapChipType mapChip = MapChipType::kBlank;
	for (Corner corner : {kRightTop, kRightBottom}) {
		auto index = mapChipField_->GetMapChipIndexSetByposition(positionsNew[corner]);
		mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

		// ===== ブロックなら通常の壁衝突 =====
		if (mapChip == MapChipType::kBlock) {
			hit = true;
			break;
		}

		// ===== トゲなら即死 =====
		if (mapChip == MapChipType::kSpike) {
			isDead_ = true; // ★右からトゲに当たったら即死
			return;
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

		// ===== ブロックなら衝突扱い =====
		if (mapChip == MapChipType::kBlock) {
			hit = true;
		}

		// ===== トゲなら即死 =====
		if (mapChip == MapChipType::kSpike) {
			isDead_ = true; // ★ 左からトゲに当たったら即死
			return;
		}
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


	

	//==================================================
	// 状態ごとの更新
	//==================================================
	if (state_ == PlayerState::Attack) {
		UpdateAttack(); // 攻撃中
	} else {
		InputMove();   // 移動・ジャンプ入力
		UpdateState(); // 状態遷移チェック（Normal⇔Rolling⇔Attack）
	}

	//==================================================
	// パーティクル（砂ぼこり）
	//==================================================
	if (onGround_ && fabs(velocity_.x) > 0.1f) {
		dustEmitTimer_ += 1.0f / 60.0f; // 経過時間を加算
		if (dustEmitTimer_ >= 0.1f) {   // 0.1秒ごとに発生
			Vector3 footPos = worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0);
			dust_.Emit(footPos);
			dustEmitTimer_ = 0.0f;
		}
	} else {
		dustEmitTimer_ = 0.0f; // 停止時はリセット
	}
	dust_.Update();

	//==================================================
	// 移動・衝突判定
	//==================================================
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	CheckMapCollision(collisionMapInfo);                   // マップ衝突判定
	worldTransform_.translation_ += collisionMapInfo.move; // 移動反映

	if (collisionMapInfo.isHitCeiling) {
		velocity_.y = 0; // 天井に当たったらY速度リセット
	}

	UpdateOnGround(collisionMapInfo); // 接地判定
	UpdateOnWall(collisionMapInfo);   // 壁判定

	//==================================================
	// 旋回処理
	//==================================================
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, 1.0f - turnTimer_ / kTimeTrun);
	}
	//==================================================
	// 穴落下死亡判定
	//==================================================
	if (worldTransform_.translation_.y < -5.0f) { // ★ここでしきい値設定
		isDead_ = true;
	}
	//==================================================
	// 行列更新
	//==================================================
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
	dust_.Draw();
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
	// --- 敵がすでに倒れているなら何もしない ---
	if (enemy->IsDefeated()) {
		return;
	}

	if (IsAttack()) {
		return; // 攻撃中は無敵
	}

	// --- それ以外は死亡 ---
	isDead_ = true;
}

void Player::OnCollision(const Enemy2* enemy) {
	// --- 敵がすでに倒れているなら何もしない ---
	if (enemy->IsDefeated()) {
		return;
	}

	if (IsAttack()) {
		return; // 攻撃中は無敵
	}

	// --- それ以外は死亡 ---
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

	// --- 攻撃開始 ---
	if (Input::GetInstance()->TriggerKey(DIK_P) && state_ == PlayerState::Normal) {
		state_ = PlayerState::Attack;
		attackPhase_ = AttackPhase::kAnticipation;
		attackTimer_ = 0;
		velocity_ = {}; // 速度リセットしてから始める
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

void Player::UpdateAttack() {
	attackTimer_++;

	switch (attackPhase_) {
	case AttackPhase::kAnticipation: {                      // 溜め（横に縮む）
		float t = static_cast<float>(attackTimer_) / 10.0f; // 10フレームで完了
		worldTransform_.scale_.x = EaseOut(1.0f, 0.5f, t);  // 横縮む
		worldTransform_.scale_.y = EaseOut(1.0f, 1.3f, t);  // 縦伸びる
		worldTransform_.scale_.z = 1.0f;

		if (attackTimer_ >= 10) {
			attackPhase_ = AttackPhase::kAction;
			attackTimer_ = 0;
		}
		break;
	}

	case AttackPhase::kAction: { // 突進（横に伸びる）
		if (lrDirection_ == LRDorection::kRight) {
			velocity_.x = +0.2f; // 速度を半分に
		} else {
			velocity_.x = -0.2f;
		}

		float t = static_cast<float>(attackTimer_) / 8.0f; // 10フレームで完了
		worldTransform_.scale_.x = EaseIn(0.5f, 1.4f, t);  // 横伸びる
		worldTransform_.scale_.y = EaseOut(1.3f, 0.7f, t); // 縦縮む
		worldTransform_.scale_.z = 1.0f;

		if (attackTimer_ >= 8) { // 10フレームで終了
			attackPhase_ = AttackPhase::kRecovery;
			attackTimer_ = 0;
		}
		break;
	}

	case AttackPhase::kRecovery: {                          // 元に戻る
		float t = static_cast<float>(attackTimer_) / 15.0f; // 15フレームで戻す
		worldTransform_.scale_.x = EaseOut(worldTransform_.scale_.x, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(worldTransform_.scale_.y, 1.0f, t);
		worldTransform_.scale_.z = 1.0f;

		velocity_.x *= 0.9f; // 減速

		if (attackTimer_ >= 15) {
			state_ = PlayerState::Normal;
			attackPhase_ = AttackPhase::kUnknown;
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f}; // 念のためリセット
		}
		break;
	}

	default:
		break;
	}

	// 衝突と移動処理は今まで通り
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	CheckMapCollision(collisionMapInfo);
	worldTransform_.translation_ += collisionMapInfo.move;

	if (collisionMapInfo.isHitCeiling)
		velocity_.y = 0;
	UpdateOnGround(collisionMapInfo);
	UpdateOnWall(collisionMapInfo);

	WorldTransformUpdate(worldTransform_);
}
void Player::CheckSpringCollision(const std::vector<std::vector<WorldTransform*>>& springs) {
	AABB playerAABB = GetAABB();

	for (auto& line : springs) {
		for (auto& spring : line) {
			if (!spring)
				continue;

			// バネの AABB を計算
			AABB springAABB = {
			    {spring->translation_.x - MapChipField::kBlockWidth / 2.0f, spring->translation_.y - MapChipField::kBlockHeight / 2.0f, -0.5f},
			    {spring->translation_.x + MapChipField::kBlockWidth / 2.0f, spring->translation_.y + MapChipField::kBlockHeight / 2.0f, +0.5f}
            };

			// 当たり判定
			if (IsCollision(playerAABB, springAABB)) {
				//  バネに触れたら強制ジャンプ
				velocity_.y = kJumpInitialVelocity * 1.5f; // 通常ジャンプの1.5倍
				onGround_ = false;
				return; // 1つヒットしたら終わり
			}
		}
	}
}
void Player::CheckFireCollision(const std::vector<std::vector<WorldTransform*>>& fires) {
	AABB playerAABB = GetAABB();

	for (auto& line : fires) {
		for (auto& fire : line) {
			if (!fire)
				continue;

			// 火の AABB
			AABB fireAABB = {
			    {fire->translation_.x - MapChipField::kBlockWidth / 2.0f, fire->translation_.y - MapChipField::kBlockHeight / 2.0f, -0.5f},
			    {fire->translation_.x + MapChipField::kBlockWidth / 2.0f, fire->translation_.y + MapChipField::kBlockHeight / 2.0f, +0.5f}
            };

			if (IsCollision(playerAABB, fireAABB)) {
				// 火に触れたら即死
				isDead_ = true;
				return;
			}
		}
	}
}
