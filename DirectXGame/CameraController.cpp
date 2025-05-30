#include "CameraController.h"
#include "Player.h"
// Al2_02_06
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }
// Al2_02_06
void CameraController::Upadate() {
	// 追従対象のワールドトランスフォームを参照// Al3_02_06
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセット空カメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() { // Al3_02_06

	// 追従対象のワールドトランスフォームを参照Al3_02_06
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセット空カメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
