#include "EnemyFlyer.h"
#include <cmath>

void EnemyFlyer::Initialize(Model* model, const Vector3& pos) {
	EnemyBase::Initialize(model, pos);

	center_ = pos; // ここを基準に飛び回る
	timer_ = 0.0f;
}

void EnemyFlyer::UpdateNormal() {
	timer_ += 0.02f; // 速度（大きくすると速くなる）

	// 円運動（XZ）
	worldTransform_.translation_.x = center_.x + std::cos(timer_) * radius_;
	

	// 上下ふわふわ
	worldTransform_.translation_.y = center_.y + std::sin(timer_ * 2.0f) * height_;
}
