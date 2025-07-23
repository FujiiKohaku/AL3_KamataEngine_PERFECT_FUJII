#include "HitEffect.h"
#include <numbers>
#include <cassert> // assert を使うなら必要

// 静的メンバ変数の定義
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

// 初期化
void HitEffect::Initialize(const KamataEngine::Vector3& position) {

	std::random_device seedGenerator;
	std::mt19937_64 randomEngine_;
	randomEngine_.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	float randomNumber = rotationDistribution(randomEngine_);

	// 必須：ワールドトランスフォームの初期化（内部で定数バッファ確保）
	circleWorldTransform_.Initialize();

	// 初期位置の設定
	circleWorldTransform_.translation_ = position;
	// 楕円エフェクト
	for (WorldTransform& worldTransform_ : elllipseWorldTransforms_) {
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		worldTransform_.rotation_ = {0.0f, 0.0f, randomNumber};
		worldTransform_.translation_ = position;

		worldTransform_.Initialize();
	}
}

// 更新
void HitEffect::Update() {
	// 回転や拡縮があればここで更新
	WorldTransformUpdate(circleWorldTransform_);
	//
	for (WorldTransform& worldTransform : elllipseWorldTransforms_) {
		WorldTransformUpdate(worldTransform);
	}
}

// 描画
void HitEffect::Draw() {
	// モデルとカメラが設定されているか確認（nullptrならクラッシュ防止）
	assert(model_ != nullptr);
	assert(camera_ != nullptr);

	model_->Draw(circleWorldTransform_, *camera_);

	for (const auto& worldTransform : elllipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}
}

// インスタンス生成
HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	// ヒットエフェクトのインスタンス生成
	HitEffect* instance = new HitEffect();

	// new失敗チェック
	assert(instance != nullptr);

	// 初期化
	instance->Initialize(position);

	return instance;
}
