#include "Enemy.h"
#include "Math.h"
#include <cmath>
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
void Enemy::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f}; 
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
	if (state_ == EnemyState::Pulled) {

		// 1) プレイヤー方向
		Vector3 dir = Normalize(target_->GetWorldTransform().translation_ - worldTransform_.translation_);

		// 2) 近づく
		worldTransform_.translation_ += dir * 0.12f;

		// 3) 小さくなる演出
		worldTransform_.scale_.x *= 0.9f;
		worldTransform_.scale_.y *= 0.9f;
		worldTransform_.scale_.z *= 0.9f;

		// 4) 近づききったら消える
		Vector3 diff = target_->GetWorldTransform().translation_ - worldTransform_.translation_;

		float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

		if (dist < 0.25f) {
			isDead_ = true;
		}
	}

	// --- 行列更新 ---
	WorldTransformUpdate(worldTransform_);
}
void Enemy::Draw(Camera* camera) {
	if (model_&&!isDead_) {
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

void Enemy::OnCollision(Player* player) { (void)player; }
void Enemy::StartPulled(Player* player) {
	state_ = EnemyState::Pulled;
	target_ = player;
}

