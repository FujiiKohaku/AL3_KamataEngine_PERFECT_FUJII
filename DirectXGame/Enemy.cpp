#include "Enemy.h"
#include "Math.h"
#include <cmath>

void Enemy::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;


	startPos_ = position; // 初期位置を記録
	direction_ = 1.0f;    // 最初は右向き
}

void Enemy::Update() {
	// 移動量
	worldTransform_.translation_.x += direction_ * kMoveSpeed;

	// 一定範囲に来たら方向反転
	if (worldTransform_.translation_.x > startPos_.x + kMoveRange) {
		direction_ = -1.0f;
	} else if (worldTransform_.translation_.x < startPos_.x - kMoveRange) {
		direction_ = 1.0f;
	}

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}
void Enemy::Draw(Camera* camera) {
	if (model_) {
		model_->Draw(worldTransform_, *camera, nullptr);
	}
}

// プレイヤーとの当たり判定
bool Enemy::CheckCollision(Player* player) const {
	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 e = worldTransform_.translation_;
	Vector3 diff = p - e;

	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	return dist < kEnemyRadius;
}

void Enemy::OnCollision(Player* player) {
	if (player) {
		player->OnCollision(this);
		// ここでは敵は消えず、動き続ける場合もOK
		// isDead_ = true;
	}
}
