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
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};

	WorldTransformUpdate(worldTransform_);
}

void JumpHopper::Update(Player* player) {

	if (cooldown_) {
		cooldownTimer_ -= 1.0f / 60.0f;
		if (cooldownTimer_ <= 0.0f)
			cooldown_ = false;
	}

	bool inside = CheckCollision(player);

	// 中に入った「瞬間」だけ発動
	if (inside && !wasInside_) {
		Activate(player);
	}

	wasInside_ = inside;

	WorldTransformUpdate(worldTransform_);
}

void JumpHopper::Draw() { model_->Draw(worldTransform_, *camera_); }

bool JumpHopper::CheckCollision(Player* player) {

	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 h = worldTransform_.translation_;

	float dx = p.x - h.x;
	float dz = p.z - h.z;

	float dist2 = dx * dx + dz * dz;
	float r = player->GetRadius() + 0.5f;
	if (dist2 > r * r)
		return false;
	float dy = p.y - h.y;
	return (dy > -0.2f && dy < 0.5f);
}


void JumpHopper::Activate(Player* player) {
	if (cooldown_)
		return;

	if (player->GetVelocity().y <= 0.0f) {

		auto v = player->GetVelocity();
		v.y = power_;
		player->SetVelocity(v);

		cooldown_ = true;
		cooldownTimer_ = kCooldownTime;
	}
}
