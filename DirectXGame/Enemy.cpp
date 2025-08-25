#define NOMINMAX
#define _USE_MATH_DEFINES
#include "Enemy.h"
#include "Player.h"
#include <algorithm> // std::max
#include <cassert>

//--------------------------------------------------
// 初期化
//--------------------------------------------------
void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// モデルとカメラの受け取り
	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();

	// 初期位置
	worldTransform_.translation_ = position;

	// 初期速度（左向きに歩かせる）
	velocity_ = {-kWalkSpeed, 0, 0};

	// 左向きに回転させる（Y軸 -90°）
	worldTransform_.rotation_.y = -float(M_PI) / 2.0f;

	// 状態リセット
	behavior_ = Behavior::kNormal;
	isDead_ = false;
	deathTimer_ = 0;
	isCollisionDisabled_ = false;
}

//--------------------------------------------------
// 更新
//--------------------------------------------------
void Enemy::UpDate() {
	if (behavior_ == Behavior::kNormal) {
		// ====== 通常行動 ======
		static float time = 0.0f;
		time += 0.05f; // アニメーション速度

		// 移動
		worldTransform_.translation_.x += velocity_.x;

		// 呼吸のようにスケール変化
		float sleepScaleY = 1.0f + 0.3f * std::sin(time);
		worldTransform_.scale_ = {1.0f, sleepScaleY, 1.0f};

		// 首振り回転
		float swing = 0.2f * std::sin(time * 2.0f);
		worldTransform_.rotation_.y = -float(M_PI) / 2.0f + swing;

	} else if (behavior_ == Behavior::kDefeated) {
		// ====== やられ演出 ======
		deathTimer_++;

		// 上に浮く
		worldTransform_.translation_.y += 0.05f;

		// 回転
		worldTransform_.rotation_.z += 0.2f;

		// 縮小
		float t = deathTimer_ / 60.0f; // 1秒くらいで消える
		float scale = std::max(0.0f, 1.0f - t);
		worldTransform_.scale_ = {scale, scale, scale};

		// 一定時間たったら本当に削除OK
		if (deathTimer_ > 60) {
			isDead_ = true; // 完全に削除してOK
		}
	}

	// 行列を更新
	WorldTransformUpdate(worldTransform_);
}

//--------------------------------------------------
// 描画
//--------------------------------------------------
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

//--------------------------------------------------
// AABB取得
//--------------------------------------------------
AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

//--------------------------------------------------
// ワールド座標取得
//--------------------------------------------------
Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

//--------------------------------------------------
// プレイヤー衝突時
//--------------------------------------------------
void Enemy::OnCollision(const Player* player) {
	// 既に死亡演出中なら無視
	if (behavior_ == Behavior::kDefeated) {
		return;
	}

	// プレイヤーが攻撃中なら倒す
	if (player->IsAttack()) {
		velocity_ = {0, 0, 0};           // 動きを止める
		behavior_ = Behavior::kDefeated; // 演出モードへ
		deathTimer_ = 0;
		isCollisionDisabled_ = true; // ★演出中は当たり判定無効
	}
}
