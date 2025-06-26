#define _USE_MATH_DEFINES
#include "Enemy.h"
#include <cassert>
#include <cmath>
#include <numbers>

// 02_10 スライド20枚目
void Enemy::OnCollision(const Player* player) { (void)player; }

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLチェック
	assert(model);

	model_ = model;

	camera_ = camera;
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// 初期座標(右側に配置)
	worldTransform_.translation_ = position;

	// 左方向を向く(プレイヤーが右向いてるよね)
	worldTransform_.rotation_.y = float(M_PI); // 180度回転

	// 02_09
	velocity_ = {-kWalkSpeed, 0, 0};

	// 02_09
	walkTimer = 0.0f;
}
void Enemy::UpDate() {
	// 02_09 16枚目 移動
	worldTransform_.translation_ += velocity_;
	// 02_09
	walkTimer += 1.0f / 60.0f;
	// 02_09 23枚目 回転アニメーション
	// --- 回転アニメーション（X軸）---
	float param = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime); // -1〜1 の周期波
	float startDegree = -15.0f;                                                             // 最初の角度（度）
	float endDegree = +15.0f;                                                               // 最後の角度（度）

	// 補間（-1～1 → 0～1 にして線形補間）
	float degree = startDegree + (endDegree - startDegree) * (param + 1.0f) / 2.0f;

	// 度をラジアンに変換
	float radian = degree * (std::numbers::pi_v<float> / 180.0f);

	// X軸の回転に適用（アニメーション）
	worldTransform_.rotation_.x = radian;
	// ワールド行列更新
	WorldTransformUpdate(worldTransform_);
}
// モデル描画
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

// 02_10 スライド14枚目
AABB Enemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}
// 02_10 スライド14枚目
Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
