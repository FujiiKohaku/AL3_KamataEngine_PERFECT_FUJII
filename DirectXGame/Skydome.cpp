#include "Skydome.h"
#include <cassert>

Skydome::Skydome() = default;
Skydome::~Skydome() = default;

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Skydome::Update() {
	// 必要なら後でスカイドームのアニメーションや回転を追加
}

void Skydome::Draw() { model_->Draw(worldTransform_, *camera_); }
