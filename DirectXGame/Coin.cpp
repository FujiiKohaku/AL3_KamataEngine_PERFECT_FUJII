#include "Coin.h"
#include "Math.h"
#include <numbers>
void Coin::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 45度傾けておく
	worldTransform_.scale_ = {0.5f * kCoinRadius, 0.5f * kCoinRadius, 0.5f * kCoinRadius}; // モデル見た目固定
	worldTransform_.translation_ = position;
}

void Coin::Update() {
	if (collected_)
		return;
	worldTransform_.rotation_.y += rotateSpeed_ * (1.0f / 60.0f) * (std::numbers::pi_v<float> / 180.0f);
	WorldTransformUpdate(worldTransform_);
}

void Coin::Draw(Camera* camera) {
	if (collected_)
		return;
	model_->Draw(worldTransform_, *camera);
}

bool Coin::CheckCollision(const Player* player) const {
	if (collected_) {
		return false;
	}

	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 c = worldTransform_.translation_;
	Vector3 diff = p - c;

	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	return dist < kCoinRadius;
}
