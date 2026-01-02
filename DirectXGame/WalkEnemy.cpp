#include "WalkEnemy.h"

void WalkEnemy::Initialize(Model* model, const Vector3& pos) {
	EnemyBase::Initialize(model, pos); // ← 共通初期化
	startPos_ = pos;
	direction_ = 1.0f;
}

void WalkEnemy::UpdateNormal() {

	

	// 左右に往復
	worldTransform_.translation_.x += direction_ * kMoveSpeed;

	if (worldTransform_.translation_.x > startPos_.x + kMoveRange) {
		worldTransform_.translation_.x = startPos_.x + kMoveRange;
		direction_ = -1.0f;
	} else if (worldTransform_.translation_.x < startPos_.x - kMoveRange) {
		worldTransform_.translation_.x = startPos_.x - kMoveRange;
		direction_ = 1.0f;
	}
}
