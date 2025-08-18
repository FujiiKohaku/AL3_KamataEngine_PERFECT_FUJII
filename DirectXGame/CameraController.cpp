#include "CameraController.h"
#include "Player.h"

#include <algorithm> // std::max, std::min を使用するために必要


void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの目的地を計算
	destination_ = targetWorldTransform.translation_ + targetOffset_;

	// 座標補完によりゆったり追従
	camera_->translation_ = Lerp(camera_->translation_, destination_, kinterpolationRate);
	
	// 移動範囲制限
	camera_->translation_.x = std::max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movableArea_.top);
	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象の和０ルドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの位置を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}