#include "Player.h"
#include "WorldRowFunction.h"
#include "imgui.h"
#include <cassert>

using namespace KamataEngine;

// 初期化
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// 引数の内容を記録
	camera_ = camera;

	// テクスチャハンドル

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

// 更新
void Player::Update() {

	// 移動入力
	//左右移動操作
	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT))
		{
	//左右加速
		Vector3 accelaration = {};
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
	
		//accelaration.x += 
		
		}
		
		
		}


	// 移動
	worldTransform_.translation_ += velocity_;
	// 行列更新
	// ここより上に処理書いて―
	WorldRowFunction::MakeAffinTransFerMatrix(worldTransform_);
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
