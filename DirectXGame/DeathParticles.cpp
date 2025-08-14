#include "DeathParticles.h"

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// モデルとカメラを保持
	model_ = model;
	camera_ = camera;

	// 各パーティクルのワールド変換を初期化して配置
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	// 色用定数バッファの初期化
	objectColor_.Initialize();

	// 初期色（白・不透明）
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {

	// ワールド行列を更新
	for (auto& worldTransform : worldTransforms_) {
		WorldTransformUpdate(worldTransform);
	}

	// 終了していたら何もしない
	if (isFinished_) {
		return;
	}

	// 寿命カウンタを進める（1フレーム分）
	counter_ += 1.0f / 60.0f;

	// 寿命が尽きたら終了フラグを立てる
	if (counter_ >= kDuration_) {
		counter_ = kDuration_;
		isFinished_ = true;
	}

	// 各パーティクルを放射状に移動させる
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本速度ベクトル
		Vector3 velocity = {kSpeed_, 0, 0};

		// i個目のパーティクルに回転角を与える
		float angle = kAngleUnit_ * i;

		// Z軸回転を適用
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);

		// 回転後の速度ベクトル
		velocity = Transform(velocity, matrixRotation);

		// 位置を更新
		worldTransforms_[i].translation_ += velocity;
	}

	// 時間経過に応じて透明にしていく（フェードアウト）
	color_.w = std::clamp(1.0f - counter_ / kDuration_, 0.0f, 1.0f);

	// 色を更新
	objectColor_.SetColor(color_);
}

void DeathParticles::Draw() {

	// 終了していたら描画しない
	if (isFinished_) {
		return;
	}

	// 各パーティクルを描画
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}
