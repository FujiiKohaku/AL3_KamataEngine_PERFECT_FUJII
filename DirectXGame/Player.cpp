#include "Player.h"
#include <cassert>

Player::Player() {}

Player::~Player() {}

// 初期化
void Player::initialize(KamataEngine::Model* model, uint32_t texturehandle, KamataEngine::Camera* camera) {

	assert(model);
	model_ = model;
	textureHandle_ = texturehandle;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::UpDate() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { 
	model_->Draw(worldTransform_, *camera_, textureHandle_);

}
