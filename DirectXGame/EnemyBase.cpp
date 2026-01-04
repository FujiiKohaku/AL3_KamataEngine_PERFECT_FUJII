#include "EnemyBase.h"
#include "KamataEngine.h"
#include "Math.h"
#include <cmath>
void EnemyBase::Initialize(Model* model, const Vector3& pos) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
}

void EnemyBase::Update() {
	if (isDead_) {
		return;
	}
	switch (state_) {

	case State::Normal:
		UpdateNormal();
		break;

	case State::Pulled:
		UpdatePulled();
		break;

	case State::Dying:
		UpdateDying(); 
		break;

	case State::Dead:
		return;
	}

	WorldTransformUpdate(worldTransform_);
}
void EnemyBase::OnCollision(Player* player) { (void)player; }

void EnemyBase::Draw(Camera* camera) {
	if (model_ && !isDead_) {
		model_->Draw(worldTransform_, *camera, nullptr);
	}
}

void EnemyBase::StartPulled(Player* player) {
	target_ = player;
	state_ = State::Pulled;

}

void EnemyBase::UpdatePulled() {

	Vector3 dir = Normalize(target_->GetWorldTransform().translation_ - worldTransform_.translation_);

	worldTransform_.translation_ += dir * 0.12f;

	worldTransform_.scale_ *= 0.9f;
	worldTransform_.rotation_.x += 0.2f;
	worldTransform_.rotation_.y += 0.2f;
	worldTransform_.rotation_.z += 0.2f;

	Vector3 diff = target_->GetWorldTransform().translation_ - worldTransform_.translation_;

	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

	if (dist < 0.25f) {
		isDead_ = true;
		state_ = State::Dead;
	}
}
void EnemyBase::StartDying() {
	state_ = State::Dying;
	deathTimer_ = 0.5f; // 演出時間(秒) 好きに調整
}
void EnemyBase::UpdateDying() {

	deathTimer_ -= 1.0f / 60.0f;

	// ふわっと上に / 回転
	worldTransform_.translation_.y += 0.03f;
	worldTransform_.rotation_.z += 0.2f;
	worldTransform_.scale_ *= 0.95f;

	if (deathTimer_ <= 0.0f) {
		isDead_ = true;
		state_ = State::Dead;
	}
}
