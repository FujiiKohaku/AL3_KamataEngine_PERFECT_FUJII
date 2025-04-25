#include "Player.h"
#include <cassert>

// 初期化
void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// 引数の内容を記録
	camera_ = camera;

	// テクスチャハンドル
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
}
// 更新
void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}
// 描画
void Player::Draw() {


	model_->Draw(worldTransform_, *camera_, textureHandle_);

	/////
}
// コンストラクタ
Player::Player() {}
// デストラクタ
Player::~Player() {}
