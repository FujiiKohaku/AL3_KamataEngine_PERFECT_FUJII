#define _USE_MATH_DEFINES
#include "Enemy.h"

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	// 左方向を向かせておく
	worldTransform_.rotation_.y = float(M_PI); // 180度
}

void Enemy::UpDate() {};

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); };
