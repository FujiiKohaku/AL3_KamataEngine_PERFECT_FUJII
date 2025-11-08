#include "BreakBlock.h"
#include <algorithm>
#include <cmath>

void BreakBlock::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	isActive_ = true;
	isBroken_ = false;
	breakTimer_ = 0.0f;
}

void BreakBlock::Update() {
	if (!isActive_) {
		return;
	}

	if (isBroken_) {
		breakTimer_ += 1.0f / 60.0f;

		// 壊れた後、時間が経てば完全に消滅
		if (breakTimer_ > kBreakDuration_) {
			isActive_ = false;
		}
	}

	// アニメーション表現（壊れてるときちょっと縮むなど）
	if (isBroken_) {
		worldTransform_.scale_ = {0.8f, 0.8f, 0.8f};
	} else {
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	}

	WorldTransformUpdate(worldTransform_);
}

void BreakBlock::Draw(Camera* camera) {
	if (!isActive_)
		return;
	if (model_) {
		model_->Draw(worldTransform_, *camera);
	}
}

bool BreakBlock::CheckCollision(const Player* player) {
	if (!isActive_) {
		return false;
	}

	Vector3 playerPos = player->GetWorldTransform().translation_;
	Vector3 blockPos = worldTransform_.translation_;

	// 簡易AABB衝突（正方形前提）
	const float half = kBlockSize_ / 2.0f;
	const float playerHalf = 0.5f; // プレイヤーの半径など

	bool hitX = std::abs(playerPos.x - blockPos.x) < (half + playerHalf);
	bool hitY = std::abs(playerPos.y - blockPos.y) < (half + playerHalf);

	return (hitX && hitY);
}

void BreakBlock::OnHit() {
	if (!isBroken_) {
		isBroken_ = true;
		breakTimer_ = 0.0f;

		// TODO: 壊れるエフェクトやSEを再生する
		// SoundManager::Play("block_break.wav");
		// particleEmitter->Emit(worldTransform_.translation_);
	}
}
