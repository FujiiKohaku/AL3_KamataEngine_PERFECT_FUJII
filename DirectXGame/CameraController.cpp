#include "CameraController.h "
#include "Math.h"
#include "Player.h"
#include <algorithm>
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

void CameraController::Update() {

	// 追従ターゲットのワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算//playerの少し先の距離を取得する
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_ + target_->GetVelocity() * kVelocityBias;
	// 座標補間によりゆったり追従 （取得した目標座標と現在のカメラ座標をLerpでなめらかに近づける）
	camera_->translation_ = Lerp(camera_->translation_, targetPosition_, kInterpolationRate);

	// 追従対象が画面買いに出ないように補正
	camera_->translation_.x = max(camera_->translation_.x, targetPosition_.x + targetMargin.left);
	camera_->translation_.x = min(camera_->translation_.x, targetPosition_.x + targetMargin.right);
	camera_->translation_.y = max(camera_->translation_.y, targetPosition_.y + targetMargin.bottom);
	camera_->translation_.y = min(camera_->translation_.y, targetPosition_.y + targetMargin.top);
	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従ターゲットのワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}