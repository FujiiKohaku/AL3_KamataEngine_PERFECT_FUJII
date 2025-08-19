#define _USE_MATH_DEFINES
#include "Enemy.h"
#include <cassert>

//--------------------------------------------------
// 初期化
//--------------------------------------------------
void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	
	// モデルとカメラの受け取り
	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();

	// 初期位置
	worldTransform_.translation_ = position;

	// 初期速度（左向きに歩かせる）
	velocity_ = { -kWalkSpeed, 0, 0 };

	// 左向きに回転させる（Y軸 -90°）
	worldTransform_.rotation_.y = -float(M_PI) / 2.0f;
}

//--------------------------------------------------
// 更新
//--------------------------------------------------
void Enemy::UpDate() {
	// 時間経過
	static float time = 0.0f;
	time += 0.05f; // アニメーション速度

	// ====== 移動 ======
	worldTransform_.translation_.x += velocity_.x;

	// ====== スケール変化（呼吸するように） ======
	float sleepScaleY = 1.0f + 0.3f * std::sin(time);
	worldTransform_.scale_ = {1.0f, sleepScaleY, 1.0f};

	// ====== 首振り回転（Y軸を基準に小刻みに揺らす） ======
	float swing = 0.2f * std::sin(time * 2.0f); // 振れ幅0.2radくらい
	worldTransform_.rotation_.y = -float(M_PI) / 2.0f + swing;

	// ====== 行列を定数バッファに転送 ======
	WorldTransformUpdate(worldTransform_);
}

//--------------------------------------------------
// 描画
//--------------------------------------------------
void Enemy::Draw() {
	model_->Draw(worldTransform_, *camera_);
}
