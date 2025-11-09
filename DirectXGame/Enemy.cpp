#include "Enemy.h"
#include "Math.h"
#include <cmath>
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
void Enemy::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f}; // 当たり判定に合わせてスケーリング
	worldTransform_.translation_ = position;


	startPos_ = position; // 初期位置を記録
	direction_ = 1.0f;    // 最初は右向き
}

void Enemy::Update() {
	// --- 一度だけ乱数初期化（もしまだなら） ---
	static bool initialized = false;
	if (!initialized) {
		srand(static_cast<unsigned int>(time(nullptr)));
		initialized = true;
	}

	// --- ランダムで方向を変える ---
	// 1/50 の確率で反転（毎フレーム判定）
	if (rand() % 50 == 0) {
		direction_ *= -1.0f;
	}

	// --- 移動 ---
	worldTransform_.translation_.x += direction_ * kMoveSpeed;

	// --- 範囲制限（はみ出したら戻す）---
	if (worldTransform_.translation_.x > startPos_.x + kMoveRange) {
		worldTransform_.translation_.x = startPos_.x + kMoveRange;
		direction_ = -1.0f;
	} else if (worldTransform_.translation_.x < startPos_.x - kMoveRange) {
		worldTransform_.translation_.x = startPos_.x - kMoveRange;
		direction_ = 1.0f;
	}

	// --- 行列更新 ---
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
