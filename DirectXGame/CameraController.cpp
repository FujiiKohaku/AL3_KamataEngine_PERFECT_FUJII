#include "CameraController.h "
#include "Player.h"
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

void CameraController::Update() {

	// 追従ターゲットのワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの目標座標を計算
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_;
	// 座標補完によりゆったり追従
	camera_->translation_ = Lerp(camera_->translation_, targetPosition_, kInterpolationRate);

	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従ターゲットのワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}