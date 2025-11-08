#include "Spike.h"
#include "Math.h"
#include <cmath>

void Spike::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f}; // サイズ調整
}

void Spike::Update() {
	if (!isActive_)
		return;
	WorldTransformUpdate(worldTransform_);
}

void Spike::Draw(Camera* camera) {
	if (!isActive_ || !model_)
		return;
	model_->Draw(worldTransform_, *camera);
}

bool Spike::CheckCollision(const Player* player) const {
	if (!isActive_)
		return false;

	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 s = worldTransform_.translation_;

	// 中心距離
	Vector3 diff = p - s;
	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

	// プレイヤーとトゲの半径和
	constexpr float kPlayerRadius = 0.5f; // Playerクラス側と合わせる
	return dist < (kPlayerRadius + kSpikeRadius);
}
