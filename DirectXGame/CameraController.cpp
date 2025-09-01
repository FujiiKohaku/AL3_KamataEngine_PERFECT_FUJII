#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

void CameraController::Update() {
	if (!target_)
		return;

	// ターゲットのワールド座標を参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// プレイヤー座標 + オフセットをカメラ位置に設定
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;

	// 行列更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	if (!target_)
		return;

	// ターゲットのワールド座標を参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 初期位置を設定
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
	camera_->UpdateMatrix();
}
