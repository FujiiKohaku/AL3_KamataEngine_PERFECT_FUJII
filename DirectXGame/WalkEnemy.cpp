#include "WalkEnemy.h"

void WalkEnemy::Initialize(Model* model, const Vector3& pos) {
	EnemyBase::Initialize(model, pos);
	startPos_ = pos;
	direction_ = 1.0f;

	hp_ = 2;
	isStaggering_ = false;
	staggerTimer_ = 0.0f;
}

void WalkEnemy::UpdateNormal() {

	// =========================
	// よろめき中の演出
	// =========================
	if (isStaggering_) {

		staggerTimer_ -= 1.0f / 60.0f;

		// ノックバック移動
		worldTransform_.translation_.x += staggerVelocity_.x;
		worldTransform_.translation_.y += staggerVelocity_.y;
		worldTransform_.translation_.z += staggerVelocity_.z;
		// 徐々に減衰
		staggerVelocity_.x *= 0.85f;
		staggerVelocity_.y *= 0.85f;

		// ブルブル回転
		worldTransform_.rotation_.z += std::sinf(staggerTimer_ * 50.0f) * 0.15f;

		if (staggerTimer_ <= 0.0f) {
			isStaggering_ = false;
			worldTransform_.rotation_.z = 0.0f;
		}

		return;
	}

	// =========================
	// 通常移動（今まで通り）
	// =========================
	worldTransform_.translation_.x += direction_ * kMoveSpeed;
	// =========================
	// マップ壁チェック
	// =========================
	if (mapChipField_) {

		Vector3 checkPos = worldTransform_.translation_;

		if (direction_ > 0.0f) {
			checkPos.x += 0.6f;
		} else {
			checkPos.x -= 0.6f;
		}

		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(checkPos);

		MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

		if (type == MapChipType::kBlock || type == MapChipType::kBreakBlock) {

			direction_ *= -1.0f;
		}
	}

	// =========================
	// 行動範囲制限
	// =========================
	if (worldTransform_.translation_.x > startPos_.x + kMoveRange) {
		worldTransform_.translation_.x = startPos_.x + kMoveRange;
		direction_ = -1.0f;
	} else if (worldTransform_.translation_.x < startPos_.x - kMoveRange) {
		worldTransform_.translation_.x = startPos_.x - kMoveRange;
		direction_ = 1.0f;
	}
}

void WalkEnemy::OnHit() {

	if (state_ == State::Dying || state_ == State::Pulled) {
		return;
	}

	hp_--;

	// =========================
	// 2回目：死亡演出
	// =========================
	if (hp_ <= 0) {
		StartDying();
		return;
	}

	// =========================
	// 1回目：よろめき開始
	// =========================
	isStaggering_ = true;
	staggerTimer_ = kStaggerTime;

	// 後ろに弾く（進行方向と逆）
	staggerVelocity_.x = -direction_ * 0.15f;
	staggerVelocity_.y = 0.05f;
	staggerVelocity_.z = 0.0f;

	staggerShakeTimer_ = kStaggerTime;
}
