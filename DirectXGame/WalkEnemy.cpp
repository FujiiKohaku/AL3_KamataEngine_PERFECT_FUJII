#include "WalkEnemy.h"

void WalkEnemy::Initialize(Model* model, const Vector3& pos) {
	EnemyBase::Initialize(model, pos); // ← 共通初期化
	startPos_ = pos;
	direction_ = 1.0f;
}

void WalkEnemy::UpdateNormal() {

	// =========================
	// 移動
	// =========================
	worldTransform_.translation_.x += direction_ * kMoveSpeed;

	// =========================
	// マップ壁チェック
	// =========================
	if (mapChipField_) {

		Vector3 checkPos = worldTransform_.translation_;

		// 進行方向の先をチェック
		if (direction_ > 0.0f) {
			checkPos.x += 0.6f;
		} else {
			checkPos.x -= 0.6f;
		}


		MapChipField::IndexSet index =mapChipField_->GetMapChipIndexSetByPosition(checkPos);

		MapChipType type =mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

		if (type == MapChipType::kBlock ||
			type == MapChipType::kBreakBlock) {

			// 方向反転
			direction_ *= -1.0f;
		}
	}

	// =========================
	// 行動範囲制限（今まで通り）
	// =========================
	if (worldTransform_.translation_.x > startPos_.x + kMoveRange) {
		worldTransform_.translation_.x = startPos_.x + kMoveRange;
		direction_ = -1.0f;
	}
	else if (worldTransform_.translation_.x < startPos_.x - kMoveRange) {
		worldTransform_.translation_.x = startPos_.x - kMoveRange;
		direction_ = 1.0f;
	}
}

