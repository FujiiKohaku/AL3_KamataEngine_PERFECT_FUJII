#include "Bullet.h"
#include "Math.h"

void Bullet::Initialize(Model* model, const Vector3& p, const Vector3& v) {

	model_ = model;
	vel_ = v;

	world_.Initialize();
	world_.translation_ = p;
}

void Bullet::Update() {

	world_.translation_ = world_.translation_ + vel_;

	WorldTransformUpdate(world_);
}

void Bullet::Draw(Camera* camera) {
	WorldTransformUpdate(world_);
	model_->Draw(world_, *camera);
}
