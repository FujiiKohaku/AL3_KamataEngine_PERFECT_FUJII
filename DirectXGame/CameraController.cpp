#include "CameraController.h"
#include "Player.h"

// 初期化：外部から渡されたカメラを保持
void CameraController::Initialize(KamataEngine::Camera* camera) {
	camera_ = camera; // Al2_02_06
}

// 毎フレーム更新（追従 → マージン制限 → 可動範囲制限 → 行列更新）
void CameraController::Upadate() {
	// 追従対象のワールドトランスフォーム参照（Al3_02_06）
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象の速度（スライド29：GetVelocityを利用）
	const Vector3& targetVelocity = target_->GetVelocity();

	// 目標座標：対象位置 + オフセット + 速度バイアス
	destination_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	// 線形補間でゆったり追従
	camera_->translation_ = Lerp(camera_->translation_, destination_, kInterpolationRate);

	// -------- 目標周りのマージン制限（スライド34）--------
	camera_->translation_.x = max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = min(camera_->translation_.y, destination_.y + targetMargin.top);

	// -------- ステージ可動範囲の制限 --------
	// X軸
	camera_->translation_.x = max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = min(camera_->translation_.x, movableArea_.right);
	// Y軸
	camera_->translation_.y = min(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = max(camera_->translation_.y, movableArea_.top);

	// ビュー/プロジェクションの更新
	camera_->UpdateMatrix();
}

// 即時リセット：対象の背後（オフセット分）へ配置
void CameraController::Reset() { // Al3_02_06
	// 追従対象のワールドトランスフォーム参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// カメラ位置 = 対象位置 + オフセット
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
