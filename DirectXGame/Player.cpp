#include "Player.h"
#include <cassert>

// 初期化
void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャハンドル
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
}
// 更新
void Player::Update() {}
// 描画
void Player::Draw() {}
// コンストラクタ
Player::Player() {}
// デストラクタ
Player::~Player() {}
