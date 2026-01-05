#include "JumpHopper.h"
#include "Math.h"
#include "Player.h"
void JumpHopper::Initialize(Model* model, Camera* camera, const Vector3& pos, float power) {
	model_ = model;
	camera_ = camera;
	power_ = power; 

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_ = {0, 0, 0};
	worldTransform_.scale_ = {1, 1, 1};

	WorldTransformUpdate(worldTransform_);
}

void JumpHopper::Update() {
	
	if (cooldown_) {
		cooldownTimer_ -= 1.0f / 60.0f;
		if (cooldownTimer_ <= 0.0f)
			cooldown_ = false;
	}

	WorldTransformUpdate(worldTransform_);
}


void JumpHopper::Draw() { model_->Draw(worldTransform_, *camera_); }

bool JumpHopper::CheckCollision(Player* player) {
	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 h = worldTransform_.translation_;

	float dx = p.x - h.x;
	float dz = p.z - h.z;

	// 半径（適当に：0.6f）
	float dist2 = dx * dx + dz * dz;
	return dist2 < (0.6f * 0.6f);
}

void JumpHopper::Activate(Player* player) {
	if (cooldown_)
		return;

	// 地面にいるときだけ
	if (player->GetVelocity().y >= 0.0f) {

		auto v = player->GetVelocity();
		v.y = power_; // ← ここ 上方向が + なら + に直す
		player->SetVelocity(v);

		cooldown_ = true;
		cooldownTimer_ = kCooldownTime;
	}
}
