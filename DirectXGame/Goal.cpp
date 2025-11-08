#include "Goal.h"
#include <cmath>

void Goal::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
}

void Goal::Update() {
	// 見た目だけ
	worldTransform_.rotation_.y += rotateSpeed_ / 60.0f;
	WorldTransformUpdate(worldTransform_);
}
bool Goal::CheckCollision(const Player* player) const {
	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 g = worldTransform_.translation_;
	Vector3 diff = p - g;

	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	return dist < kGoalRadius;
}
void Goal::Draw(Camera* camera) { model_->Draw(worldTransform_, *camera); }
