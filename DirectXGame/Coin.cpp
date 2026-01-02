#include "Coin.h"
#include "Math.h"
#include <numbers>
#include <Windows.h>
void Coin::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 45度傾けておく
	worldTransform_.scale_ = {0.5f * kCoinRadius, 0.5f * kCoinRadius, 0.5f * kCoinRadius}; // モデル見た目固定
	worldTransform_.translation_ = position;
	//char buf[128];
	//sprintf_s(buf, "Coin Init pos = (%f, %f, %f)\n", position.x, position.y, position.z);

	//OutputDebugStringA(buf); // ← これ！

}

void Coin::Update() {

	switch (state_) {

	case State::Normal:
		// いつもの回転
		worldTransform_.rotation_.y += 0.05f;
		break;

	case State::Pulled: {

		Vector3 dir = Normalize(target_->GetWorldTransform().translation_ - worldTransform_.translation_);

		worldTransform_.translation_ += dir * 0.15f;

		worldTransform_.scale_ *= 0.92f;

		Vector3 diff = target_->GetWorldTransform().translation_ - worldTransform_.translation_;

		float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

		if (dist < 0.25f) {
			collected_ = true;
			state_ = State::Dead;
		}
		break;
	}

	case State::Dead:
		return;
	}

	WorldTransformUpdate(worldTransform_);
}


void Coin::Draw(Camera* camera) {
	if (collected_)
		return;
	model_->Draw(worldTransform_, *camera);
}

bool Coin::CheckCollision(const Player* player) const {
	if (collected_)
		return false;
	if (state_ == State::Pulled)
		return false; 

	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 c = worldTransform_.translation_;
	Vector3 diff = p - c;

	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	return dist < kCoinRadius;
}

void Coin::StartPulled(Player* player) {
	target_ = player;
	state_ = State::Pulled;
}

void Coin::Collect() {
	collected_ = true;
	state_ = State::Dead;
}
