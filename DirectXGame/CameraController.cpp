#include "CameraController.h"
#include "Player.h"

#include <algorithm> // std::max, std::min を使用するために必要
CameraController* CameraController::instance_ = nullptr;

CameraController* CameraController::GetInstance() {
	if (!instance_) {
		instance_ = new CameraController();
	}
	return instance_;
}
void CameraController::Finalize() {
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

void CameraController::Update() {

// =========================
	// ラピッドショット・カットイン
	// =========================
	if (state_ == CameraState::RapidShotCutInIn || state_ == CameraState::RapidShotCutInOut) {

		cutInTimer_ -= 1.0f / 60.0f;

		float t = 1.0f - (cutInTimer_ / cutInDuration_);
		t = std::clamp(t, 0.0f, 1.0f);

		// EaseOutCubic
		float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

		if (state_ == CameraState::RapidShotCutInIn) {
			// 元 → 寄る
			camera_->translation_ = Lerp(cutInStartPos_, cutInTargetPos_, eased);
		} else {
			// 寄る → 元
			camera_->translation_ = Lerp(cutInTargetPos_, cutInStartPos_, eased);
		}

		camera_->UpdateMatrix();

		if (cutInTimer_ <= 0.0f) {

			if (state_ == CameraState::RapidShotCutInIn) {
				// 後半（戻り）へ
				state_ = CameraState::RapidShotCutInOut;
				cutInTimer_ = cutInDuration_;
			} else {
				// 完全終了
				state_ = CameraState::Normal;
			}
		}

		return;
	}

	// プレイヤーの現在位置を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// プレイヤーの現在速度を取得
	const KamataEngine::Vector3& targetvelocity = target_->GetVelocity();

	// カメラの目的地 = プレイヤー位置 + オフセット + (速度 × バイアス)
	destination_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetvelocity.x * kVelocityBias;
	destination_.y = camera_->translation_.y; // ← ジャンプしてもYは動かさない
	destination_.z = targetWorldTransform.translation_.z + targetOffset_.z;


	// 補間でなめらかに目的地へ近づける（ゆったり追従）
	camera_->translation_ = Lerp(camera_->translation_, destination_, kinterpolationRate);

	// プレイヤーとのマージン制御（画面からはみ出さないように）
	camera_->translation_.x = std::max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = std::min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = std::max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, destination_.y + targetMargin.top);

	// ステージ全体の移動範囲制限（外に出ないように）
	camera_->translation_.x = std::max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movableArea_.top);

	if (shakeTimer_ > 0.0f) {
		shakeTimer_ -= 1.0f / 60.0f;

		// ランダムに揺らす（XZ）
		float sx = ((float)rand() / RAND_MAX - 0.5f) * shakePower_;
		float sy = ((float)rand() / RAND_MAX - 0.5f) * shakePower_;

		camera_->translation_.x += sx;
		camera_->translation_.z += sy;
	}
	// 最終的に行列を更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象の和０ルドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの位置を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
void CameraController::StartShake(float power, float time) {
	shakePower_ = power;
	shakeTimer_ = time;
}
void CameraController::StartRapidShotCutIn() {

	state_ = CameraState::RapidShotCutInIn;
	cutInTimer_ = cutInDuration_;

	// 元の位置を保存
	cutInStartPos_ = camera_->translation_;

	// 寄る先
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	cutInTargetPos_ = targetWorldTransform.translation_ + Vector3(0.0f, 0.6f, -3.5f);
}
