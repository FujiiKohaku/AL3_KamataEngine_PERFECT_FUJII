#include "BlockBreakEffect.h"
void BlockBreakEffect::Initialize(Model* model, const Vector3& position) {
	blockBreakEffect_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	timer_ = lifeTime_;
	// ランダムに飛ばす
	velocity_.x = (rand() % 200 - 100) / 100.0f * 0.1f;
	velocity_.y = (rand() % 200) / 100.0f * 0.15f;
	velocity_.z = (rand() % 200 - 100) / 100.0f * 0.1f;

	rotateVelocity_.x = (rand() % 200 - 100) / 100.0f * 0.1f;
	rotateVelocity_.y = (rand() % 200 - 100) / 100.0f * 0.1f;
	rotateVelocity_.z = (rand() % 200 - 100) / 100.0f * 0.1f;
}


void BlockBreakEffect::Update() {
	timer_ -= 1.0f / 60.0f;

	// 移動
	worldTransform_.translation_ += velocity_;

	// 重力
	velocity_.y -= 0.01f;

	// 回転
	worldTransform_.rotation_.x += rotateVelocity_.x;
	worldTransform_.rotation_.y += rotateVelocity_.y;
	worldTransform_.rotation_.z += rotateVelocity_.z;

	WorldTransformUpdate(worldTransform_);
}


void BlockBreakEffect::Draw(Camera* camera) {
	
	
	blockBreakEffect_->Draw(worldTransform_,*camera,nullptr);

}

bool BlockBreakEffect::IsAlive() const { return timer_ > 0.0f; }
