#include "Block.h"

void Block::Initialize(Model* model, const Vector3& position, bool isBreakable) {
	model_ = model;
	isBreakable_ = isBreakable;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	WorldTransformUpdate(worldTransform_);
}

void Block::Update() {
	// 今は何もしない
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
	if (isBroken_) {
		return;
	}

	isBroken_ = true;
	justBroken_ = true;
}

bool Block::IsBroken() const { return isBroken_; }

bool Block::IsBreakable() const { return isBreakable_; }

const WorldTransform& Block::GetWorldTransform() const { return worldTransform_; }

float Block::GetRadius() const { return 0.5f; }

bool Block::CheckHitSphere(const Vector3& pos, float radius) const {
	if (isBroken_) {
		return false;
	}

	Vector3 blockPos = worldTransform_.translation_;
	Vector3 diff = pos - blockPos;

	float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	float hit = radius + GetRadius();
	return distSq < hit * hit;
}

void Block::SetMapIndex(uint32_t x, uint32_t y) {
	mapX_ = x;
	mapY_ = y;
}

uint32_t Block::GetMapX() const { return mapX_; }

uint32_t Block::GetMapY() const { return mapY_; }

bool Block::IsJustBroken() const { return justBroken_; }

void Block::ClearJustBroken() { justBroken_ = false; }
