#include "Skydome.h"

void Skydome::initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {

	
	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
}

void Skydome::UpDate() { worldTransform_.TransferMatrix(); }

void Skydome::Draw() {

	model_->Model::Draw(worldTransform_, *camera_); // テクスチャなしで描画
}
