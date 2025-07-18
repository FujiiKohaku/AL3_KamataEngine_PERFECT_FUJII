#include "HitEffect.h"
// 初期化
void HitEffect::Initialize(const KamataEngine::Vector3& position) {};
// 更新
void HitEffect::Update() {

	// 円形エフェクト
	WorldTransformUpdate(circleWorldTransform_);
};

// 静的メンバ変数の実体
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;
// 描画
void HitEffect::Draw() {


	HitEffect::model_->Draw(circleWorldTransform_, *camera_); };

HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	// インスタンス生成委
	HitEffect* instance = new HitEffect();
	// newの失敗を検出
	assert(instance != nullptr);
	// インスタンスの初期化
	instance->Initialize(position);
	// 初期化したインスタんうすを返す
	return instance;
}