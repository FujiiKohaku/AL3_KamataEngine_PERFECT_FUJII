#include "HitEffect.h"
#include <cassert> // assert使用のため
#include <numbers>

// ==============================
// 静的メンバ変数の定義
// ==============================
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

// ==============================
// 初期化
// ==============================
void HitEffect::Initialize(const KamataEngine::Vector3& position) {
	// 乱数生成器（回転角用）
	std::random_device seedGenerator;
	std::mt19937_64 randomEngine_;
	randomEngine_.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	float randomNumber = rotationDistribution(randomEngine_);

	// 円形（中心）エフェクトのワールド変換初期化
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;

	// 楕円エフェクト用ワールド変換の初期化
	for (WorldTransform& worldTransform_ : elllipseWorldTransforms_) {
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		worldTransform_.rotation_ = {0.0f, 0.0f, randomNumber};
		worldTransform_.translation_ = position;
		worldTransform_.Initialize();
	}
}

// ==============================
// 更新
// ==============================
void HitEffect::Update() {
	// 円形（中心）の行列更新
	WorldTransformUpdate(circleWorldTransform_);

	// 楕円エフェクトの行列更新
	for (WorldTransform& worldTransform : elllipseWorldTransforms_) {
		WorldTransformUpdate(worldTransform);
	}
}

// ==============================
// 描画
// ==============================
void HitEffect::Draw() {
	// モデルとカメラが設定されているかチェック
	assert(model_ != nullptr);
	assert(camera_ != nullptr);

	// 円形エフェクト描画
	model_->Draw(circleWorldTransform_, *camera_);

	// 楕円エフェクト描画
	for (const auto& worldTransform : elllipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}
}

// ==============================
// インスタンス生成
// ==============================
HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	// 動的生成
	HitEffect* instance = new HitEffect();
	assert(instance != nullptr); // new失敗チェック

	// 初期化
	instance->Initialize(position);

	return instance;
}
