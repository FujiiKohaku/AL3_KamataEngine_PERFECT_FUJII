#include "PlayerBullet.h"
#include "Math.h"
void PlayerBullet::Initialize(Model* model, const Vector3& pos, const Vector3& dir) {
	model_ = model;
	world_.Initialize();
	world_.translation_ = pos;
	direction_ = dir;
	speed_ = 0.3f;
	alive_ = true;
	world_.scale_ = {0.5f, 0.5f, 0.5f};
}

void PlayerBullet::Update() {
	if (!alive_)
		return;
	world_.translation_.x += direction_.x * speed_;
	world_.translation_.y += direction_.y * speed_;
	world_.translation_.z += direction_.z * speed_;

	
		//alive_ = false;
	

	WorldTransformUpdate(world_);
}

void PlayerBullet::Draw(Camera* camera) {
	if (!alive_)
		return;
	model_->Draw(world_, *camera);
}
