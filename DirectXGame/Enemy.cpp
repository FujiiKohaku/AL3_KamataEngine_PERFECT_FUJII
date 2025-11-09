#include "Enemy.h"
#include <cmath>
#include"Math.h"
void Enemy::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Enemy::Update() {
	// とりあえず動かない敵（静止）
	// 例: あとで左右移動など追加してOK
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

	// 当たっているなら true
	return dist < kEnemyRadius;
}

// プレイヤーとぶつかった時の処理
void Enemy::OnCollision(Player* player) {
	if (player) {
		// 双方向例: プレイヤーにダメージ
		player->OnCollision(this);
		isDead_ = true; // 例えば倒された扱い
	}
}
