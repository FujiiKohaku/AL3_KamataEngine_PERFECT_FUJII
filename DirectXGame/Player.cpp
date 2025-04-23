#include "Player.h"
#include <cassert>

// 初期化
void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	// テクスチャハンドル
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
}
// 更新
void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();//ここ最後
}
// 描画
void Player::Draw() {}
// コンストラクタ
Player::Player() {}
// デストラクタ
Player::~Player() {}
