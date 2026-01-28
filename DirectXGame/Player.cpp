#define NOMINMAX
#include "Player.h"
#include "CameraController.h"
#include "Coin.h"

#include "Goal.h"
#include "MapChipField.h"
#include "Math.h"
#include "PlayerBullet.h"
#include "Spike.h"
#include <algorithm>
#include <cassert>
#include <numbers>

#include "EnemyBase.h"
void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {

	assert(model);
	// モデル
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	camera_ = camera;

	bulletModel_ = Model::CreateFromOBJ("star", true);

	inhaleEffect_.Initialize();

	shotSeHandle_ = Audio::GetInstance()->LoadWave("shoot.wav");
}
void Player::Shoot() {

	if (vacuumPoint_ <= 0) {
		return;
	}

	std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();

	Vector3 dir{};
	if (lrDirection_ == LRDirection::kRight) {
		dir.x = 1.0f;
	} else {
		dir.x = -1.0f;
	}

	bullet->Initialize(bulletModel_, worldTransform_.translation_, dir);

	bullets_.push_back(std::move(bullet));

	vacuumPoint_ = 0;
	jumpCount_ = 0;
}

void Player::InputMove() {

	Vector3 acceleration{};

	// ========================
	// 横移動（回転処理は残す）
	// ========================
	if (Input::GetInstance()->PushKey(DIK_D)) {

		if (velocity_.x < 0.0f) {
			velocity_.x *= (1.0f - kAttenuation);
		}

		acceleration.x += kAcceleration / 60.0f;

		if (lrDirection_ != LRDirection::kRight) {
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}

	} else if (Input::GetInstance()->PushKey(DIK_A)) {

		if (velocity_.x > 0.0f) {
			velocity_.x *= (1.0f - kAttenuation);
		}

		acceleration.x -= kAcceleration / 60.0f;

		if (lrDirection_ != LRDirection::kLeft) {
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}

	} else {
		velocity_.x *= (1.0f - kAttenuation);
	}

	// ========================
	// 加速の重さ
	// ========================
	if (!onGround_) {
		acceleration.x *= 0.5f;
	}

	if (vacuumPoint_ > 0) {
		acceleration.x *= 0.35f; // とことこ感
	}

	// ========================
	// 速度更新
	// ========================
	velocity_ += acceleration;

	float runLimit = kLimitRunSpeed;
	if (vacuumPoint_ > 0) {
		runLimit *= 0.18f; // 最高速度を下げる
	}

	velocity_.x = std::clamp(velocity_.x, -runLimit, runLimit);

	if (std::abs(velocity_.x) <= 0.0001f) {
		velocity_.x = 0.0f;
	}

	// ========================
	// ジャンプ制限（←ここが本題）
	// ========================
	int jumpLimit = kMaxJumpCount;

	if (vacuumPoint_ == 1) {
		jumpLimit = 1; // 1体 → 1段だけ
	} else if (vacuumPoint_ >= 2) {
		jumpLimit = 0; // 2体 → 完全禁止
	}

	bool canJump = (jumpCount_ < jumpLimit);

	if (canJump && Input::GetInstance()->TriggerKey(DIK_W)) {

		float jumpPower = kJumpAcceleration;

		if (jumpCount_ > 0) {
			jumpPower *= kSecondJumpPowerScale;
			spinActive_ = true;
			spinTimer_ = 0.0f;
		}

		velocity_.y = jumpPower / 60.0f;
		jumpCount_++;
		onGround_ = false;
	}

	// ========================
	// 重力
	// ========================
	if (!onGround_) {
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		Shoot();
		Audio::GetInstance()->PlayWave(shotSeHandle_,false,1.0f);
	}
	for (auto it = bullets_.begin(); it != bullets_.end();) {

		(*it)->Update();

		if (!(*it)->IsAlive()) {
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

	Vector3 inhaleCenter = worldTransform_.translation_;

	if (lrDirection_ == LRDirection::kRight) {
		inhaleCenter.x += 1.0f;
	} else {
		inhaleCenter.x -= 1.0f;
	}

	inhaleEffect_.Update(inhaleCenter, state_ == PlayerState::Inhale);

	bobbingTime_ += 0.1f;
	worldTransform_.scale_.y = baseScale + sin(bobbingTime_) * 0.2f;
	//==========================
	//  死亡演出状態の処理
	//==========================
	if (deathState_ == DeathState::Dying) {

		// 演出時間更新
		deathTimer_ -= 1.0f / 60.0f;

		// 浮く演出 → 落下
		worldTransform_.translation_ += deathVelocity_;
		deathVelocity_.y -= 0.01f;

		// 回転演出
		worldTransform_.rotation_.z += 0.15f;
		worldTransform_.rotation_.x += 0.05f;

		// DirectX に転送
		WorldTransformUpdate(worldTransform_);

		// 演出が終わったら完全死亡
		if (deathTimer_ <= 0.0f) {
			deathState_ = DeathState::DeadFinish;
		}

		return;
	}

	//==========================
	// ゴール時も通常処理しない
	//==========================
	if (isGoal_) {
		return;
	}

	//==========================
	//  生存中の通常処理
	//==========================
	InputMove();

	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	CheckMapCollision(collisionMapInfo);

	worldTransform_.translation_ += collisionMapInfo.move;

	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}
	// 無敵中
	if (invincible_) {
		invincibleTimer_ -= (1.0f / 60.0f);

		// 点滅（描画スキップ）
		if (((int)(invincibleTimer_ * 10) % 2) == 0) {
			visible_ = false;
		} else {
			visible_ = true;
		}

		if (invincibleTimer_ <= 0) {
			invincible_ = false;
			visible_ = true;
		}
	}
	UpdateOnWall(collisionMapInfo);
	UpdateOnGround(collisionMapInfo);

	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	//==========================
	//  落下死を演出に変える
	//==========================
	if (worldTransform_.translation_.y < -20.0f) {
		StartDeath();
	}
	//=====================================================
	// 二段ジャンプ演出： X軸一回転（EaseOut）
	//=====================================================
	if (spinActive_) {

		// 経過時間更新
		spinTimer_ += 1.0f / 60.0f;

		// 0 ~ 1 に正規化
		float t = std::clamp(spinTimer_ / spinDuration_, 0.0f, 1.0f);

		// イージング適用
		float eased = EaseOutCubic(t);

		// 1回転分(360°)を X軸回転
		worldTransform_.rotation_.x = eased * (std::numbers::pi_v<float> * 2.0f);

		if (t >= 1.0f) {
			spinActive_ = false;
		}
	}

	bool now = Input::GetInstance()->PushKey(DIK_SPACE);


	// すでに何か吸ってたら吸い込み不可
	if (vacuumPoint_ > 0) {
		StopInhale();
	} else {
		if (now) {
			StartInhale();
		} else {
			StopInhale();
		}
	}


	inhaling_ = now;

	Vector3 offset;

	if (lrDirection_ == LRDirection::kRight) {

		offset = Vector3(+0.8f, 0, 0);
	} else {

		offset = Vector3(-0.8f, 0, 0);
	}
	inhaleHitBox_.pos = worldTransform_.translation_ + offset;

	WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {

	if (visible_) {
		model_->Draw(worldTransform_, *camera_);
	}
	
	for (auto& b : bullets_) {
		b->Draw(camera_);
	}
	
	
	
}

void Player::OnCollision(Coin* coin) {
	if (coin) {
		// coin->SetCollected(true);
		//  例: コイン取得音を再生
		//  SoundManager::GetInstance()->PlaySE("coin");
	}
}

void Player::OnCollision(Spike* spike) {
	if (spike) {
		StartDeath();
	}
}

void Player::OnCollision(Goal* goal) {
	if (goal) {
		isGoal_ = true;
		// 例: ゴールSE
		// SoundManager::GetInstance()->PlaySE("goal");
	}
}

void Player::OnCollision(EnemyBase* enemy) {
	if (!enemy)
		return;

	Vector3 enemyPos = enemy->GetWorldTransform().translation_;
	Vector3 playerPos = worldTransform_.translation_;

	// =========================
	// すでに吸われ中の敵は最優先で処理
	// =========================
	if (enemy->IsPulled()) {

		// 吸い込み中なら確定で取り込む
		if (state_ == PlayerState::Inhale) {
			AbsorbEnemy(enemy);
		}

		return;
	}

	// =========================
	// 吸い込み中：上下からの接触は無視
	// （まだ吸われていない敵だけ）
	// =========================
	if (state_ == PlayerState::Inhale) {

		float dy = enemyPos.y - playerPos.y;
		if (std::abs(dy) > 0.5f) {
			return;
		}
	}

	// =========================
	// 向き判定
	// =========================
	bool enemyInFront = false;

	if (lrDirection_ == LRDirection::kRight) {
		if (enemyPos.x > playerPos.x) {
			enemyInFront = true;
		}
	} else {
		if (enemyPos.x < playerPos.x) {
			enemyInFront = true;
		}
	}

	// =========================
	// 後ろから来た敵はダメージ
	// =========================
	if (!enemyInFront) {
		if (!invincible_) {
			TakeDamage(enemyPos);
		}
		return;
	}

	if (invincible_) {
		return;
	}

	switch (state_) {
	case PlayerState::Normal:
		TakeDamage(enemyPos);
		break;

	case PlayerState::Inhale:
		AbsorbEnemy(enemy);
		break;

	case PlayerState::Swallow:
		break;
	}
}



void Player::AbsorbEnemy(EnemyBase* enemy) {
	if (!enemy)
		return;

	enemy->StartPulled(this); // Enemyに吸い寄せを命令
	vacuumPoint_ = std::min(vacuumPoint_ + 1, kMaxVacuum);
}

/////////////////////////////////////////////////
// 死亡開始
/////////////////////////////////////////////////
void Player::StartDeath() {

	if (deathState_ != DeathState::Alive)
		return;

	deathState_ = DeathState::Dying;
	deathTimer_ = 1.5f;
	deathVelocity_ = {0, 0.25f, 0};
	velocity_ = {0, 0, 0}; // ゲーム操作の速度は無効
}
float Player::EaseOutCubic(float t) { // t = [0.0f, 1.0f]
	float inv = 1.0f - t;
	return 1.0f - inv * inv * inv;
}
void Player::TakeDamage(const Vector3& enemyPos) {

	hp_--;

	// ノックバック
	Vector3 dir = worldTransform_.translation_ - enemyPos;
	dir.y = 0;
	dir = Normalize(dir);
	velocity_ = dir * 0.2f; // ノックバック強さ

	// 無敵開始
	invincible_ = true;
	invincibleTimer_ = 1.0f; // 1秒間無敵

	// HP0なら死亡
	if (hp_ <= 0) {
		StartDeath();
	}
	CameraController::GetInstance()->StartShake(0.5f, 0.1f);
}
void Player::StartInhale() {
	state_ = PlayerState::Inhale;
	inhaleHitBox_.active = true; 
}
void Player::StopInhale() {
	state_ = PlayerState::Normal;
	inhaleHitBox_.active = false; 
}
#pragma region マップの当たり判定をまとめたもの
// 02_07 スライド13枚目
void Player::CheckMapCollision(CollisionMapInfo& info) {

	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

#pragma endregion

#pragma region 上の処理
// 02_07 スライド14枚目(上下左右全て)
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	// 02_07スライド20枚目 上昇あり?
	if (info.move.y <= 0) {
		return;
	}

	// 02_07 スライド19枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 02_07 スライド28枚目（下のfor文も）
	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？ 02_07 スライド34枚目
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			info.ceiling = true;
		}
	}
}

#pragma endregion

#pragma region 下の処理

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	info;

	// 02_08 スライド7枚目 下降あり？
	if (info.move.y >= 0) {
		return;
	}

	// 02_08 スライド7枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 02_08 スライド8枚目(右下、左下の判定まで)
	MapChipType mapChipType;

	// フラグ初期化
	bool hit = false;

	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));

		// 現在座標（＝移動前の下端座標）が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));

		// 移動前後でマップチップのYインデックスが違う場合だけ補正
		if (indexSetNow.yIndex != indexSet.yIndex) {

			// めり込み先ブロックの範囲矩形を取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// 移動量を修正（下から当たったので上に押し戻す）
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));

			// 地面に当たったことを記録
			info.landing = true;
		}
	}
}
#pragma endregion

#pragma region 地上にいるときの処理

// 02_08スライド14枚目 設置状態の切り替え処理
void Player::UpdateOnGround(const CollisionMapInfo& info) {

	if (onGround_) {

		// 上向き速度が出たら空中へ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			// まだ地面ある？
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			bool hit = false;

			// 左下
			auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
				hit = true;
			}

			// 右下
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
				hit = true;
			}

			// 足元が無ければ落下
			if (!hit) {
				onGround_ = false;
			}
		}

	} else {

		// 下から当たった時だけ着地扱い
		if (info.landing && velocity_.y <= 0.0f) {

			onGround_ = true;

			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;

			jumpCount_ = 0;

			// 姿勢リセット
			worldTransform_.rotation_.x = 0.0f;
			spinActive_ = false;
		}
	}
}

#pragma endregion

#pragma region 壁当たっているときの処理
// 02_08スライド27枚目 壁接地中の処理
void Player::UpdateOnWall(const CollisionMapInfo& info) {

	if (!info.hitWall)
		return;

	// 地上の減速（そのまま）
	if (onGround_) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}

	if (lrDirection_ == LRDirection::kLeft) {
		if (velocity_.x < 0.0f)
			velocity_.x = 0.0f;
	}

	if (lrDirection_ == LRDirection::kRight) {
		if (velocity_.x > 0.0f)
			velocity_.x = 0.0f;
	}
}
#pragma endregion

#pragma region 右の当たり判定
// 中身入れるのは02_08スライド25枚目
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {

		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));

		if (indexSetNow.xIndex != indexSet.xIndex) {

			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));

			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));

			info.hitWall = true;

			if (onGround_) {
				velocity_.x = 0.0f;
			}

			const float kWallGap = 0.09f;
			info.move.x -= kWallGap;
		}
	}
}

#pragma endregion

#pragma region 左の当たり判定
// 中身入れるのは02_08スライド25枚目
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {

		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		if (indexSetNow.xIndex != indexSet.xIndex) {

			// めり込み先ブロックの範囲取得
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));

			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// めり込み補正
			info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));

			info.hitWall = true;

			// 地上止める
			if (onGround_) {
				velocity_.x = 0.0f;
			}

			//   ほんの少しだけ壁から離す
			const float kWallGap = 0.09f;
			info.move.x += kWallGap;
		}
	}
}

#pragma endregion

#pragma region 角の位置を取得
// 02_07 スライド17枚目
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

#pragma endregion