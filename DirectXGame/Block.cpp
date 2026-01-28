#include "Block.h"

void Block::Initialize(Model* model, const Vector3& position, bool isBreakable) {
	model_ = model;
	isBreakable_ = isBreakable;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	WorldTransformUpdate(worldTransform_);
}

void Block::Update() {
	if (isBroken_) {
		return;
	}
}

void Block::Draw(Camera* camera) {
	if (isBroken_) {
		return;
	}
	model_->Draw(worldTransform_, *camera, nullptr);
}

void Block::Break() {
	if (!isBreakable_) {
		return;
	}
	isBroken_ = true;
}

bool Block::IsBroken() const { return isBroken_; }

bool Block::IsBreakable() const { return isBreakable_; }

const WorldTransform& Block::GetWorldTransform() const { return worldTransform_; }
