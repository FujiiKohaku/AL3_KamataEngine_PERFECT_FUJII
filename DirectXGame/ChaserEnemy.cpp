#include "ChaserEnemy.h"
#include <cmath>

#include "Math.h"
void ChaserEnemy::Initialize(Model* model, const Vector3& pos, Player* player) {
	EnemyBase::Initialize(model, pos);
	player_ = player;
}

void ChaserEnemy::UpdateNormal() {
	if (!player_)
		return;

	Vector3 diff = player_->GetWorldTransform().translation_ - worldTransform_.translation_;

	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

	// 範囲内で追尾開始
	if (dist < detectRange_)
		chasing_ = true;

	// 追尾中
	if (chasing_) {

		// 向き（正規化）
		Vector3 dir = Normalize(diff);

		// 少しずつ近づく
		worldTransform_.translation_ += dir * chaseSpeed_;

		// かなり離れたらやめる（好みで）
		if (dist > detectRange_ * 1.5f)
			chasing_ = false;
	}

	WorldTransformUpdate(worldTransform_);
}
