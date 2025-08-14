#define _USE_MATH_DEFINES
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include <cassert>
#include <cmath>
#include <numbers>

// ==============================
// 衝突コールバック（プレイヤーとの接触）
// ==============================
void Enemy::OnCollision(const Player* player) {
	// 既に敗北状態なら何もしない
	if (behavior_ == Behavior::kDefeated) {
		return;
	}

	// プレイヤーが攻撃中なら敗北リクエスト＆当たり判定無効化
	if (player->IsAttack()) {
		behaviorRequest_ = Behavior::kDefeated;
		isCollisionDisabled_ = true;

		// ヒットエフェクトを生成
		Vector3 effectPos;
		effectPos.x = static_cast<float>(GetWorldPosition().x);
		effectPos.y = static_cast<float>(GetWorldPosition().y);
		effectPos.z = static_cast<float>(GetWorldPosition().z);
		gameScene_->CreateHitEffect(effectPos);

		(void)player; // 未使用警告抑制
	}
}

// ==============================
// 初期化
// ==============================
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);

	model_ = model;
	camera_ = camera;

	// ワールド変換
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 左向きに（Y軸180度）
	worldTransform_.rotation_.y = float(M_PI);

	// 動作パラメータ初期値
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer = 0.0f;
}

// ==============================
// 毎フレーム更新
// ==============================
void Enemy::UpDate() {
	// ふるまい変更リクエストがあれば反映
	if (behaviorRequest_ != Behavior::kUnnown) {
		behavior_ = behaviorRequest_;

		// 状態ごとの初期化
		switch (behavior_) {
		case Enemy::Behavior::kDefeated:
		default:
			counter_ = 0; // 汎用カウンタをリセット
			break;
		}
		behaviorRequest_ = Behavior::kUnnown;
	}

	// 現在のふるまいに応じた処理
	switch (behavior_) {
	case Enemy::Behavior::kWalk:
		// 移動＆歩行アニメ（回転）
		worldTransform_.translation_ += velocity_;
		walkTimer += 1.0f / 60.0f;
		worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

		// ワールド行列更新
		WorldTransformUpdate(worldTransform_);
		break;

	case Enemy::Behavior::kDefeated:
		// 敗北演出：回転＋イージング傾き
		counter_ += 1.0f / 60.0f;
		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = EaseOut(ToRadians(kDefeatedMotionAngleStart), ToRadians(kDefeatedMotionAngleEnd), counter_ / kDefeatedTime);

		WorldTransformUpdate(worldTransform_);

		// 演出終了で死亡フラグ
		if (counter_ >= kDefeatedTime) {
			isDead_ = true;
		}
		break;

	default:
		break;
	}

	// 最終的にワールド行列を更新（重複呼び出しは元コード踏襲）
	WorldTransformUpdate(worldTransform_);
}

// ==============================
// 描画
// ==============================
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

// ==============================
// AABB取得（当たり判定用）
// ==============================
AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

// ==============================
// ワールド座標取得
// ==============================
Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
