#include "CameraController.h"
#include "Player.h"

// Al2_02_06
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

// Al2_02_06
void CameraController::Upadate() {
	// 追従対象のワールドトランスフォームを参照 // Al3_02_06
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 02_06 スライド29（追従対象の速度）
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象 + オフセット + 速度バイアス で目標座標
	destination_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	// 座標補間でゆったり追従
	camera_->translation_ = Lerp(camera_->translation_, destination_, kInterpolationRate);

	// 移動範囲制限（ターゲット周りのマージン） 02_06 スライド34
	camera_->translation_.x = max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = min(camera_->translation_.y, destination_.y + targetMargin.top);

	// 移動範囲制限（カメラ全体の可動範囲）
	// X軸
	camera_->translation_.x = max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = min(camera_->translation_.x, movableArea_.right);
	// Y軸
	camera_->translation_.y = min(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = max(camera_->translation_.y, movableArea_.top);

	// 行列更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() { // Al3_02_06
	// 追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象 + オフセットでカメラ座標を設定
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
