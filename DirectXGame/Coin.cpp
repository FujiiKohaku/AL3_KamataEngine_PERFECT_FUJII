#include "Coin.h"
#include "Math.h"

void Coin::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Coin::Update() {
	if (collected_)
		return;

	WorldTransformUpdate(worldTransform_);
}

void Coin::Draw(Camera* camera) {
	if (collected_)
		return; // 取得済みなら描画しない
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
