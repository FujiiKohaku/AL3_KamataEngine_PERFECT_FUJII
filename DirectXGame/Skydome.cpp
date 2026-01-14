#include "Skydome.h"
#include <cassert>
void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {

	// モデルを初期化でゲームシーンから取ってくる
	model_ = model;
	// カメラを初期化でゲームシーンから取ってくる
	camera_ = camera;
	// ワールド座標変換に初期化
	worldTransform_.Initialize();
}

void Skydome::Update() { 
	
	worldTransform_.rotation_.y += 0.001f;
	
	WorldTransformUpdate(worldTransform_); }

void Skydome::Draw() { model_->Draw(worldTransform_, *camera_); }