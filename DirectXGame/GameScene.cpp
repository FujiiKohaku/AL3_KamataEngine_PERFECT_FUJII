#include "GameScene.h"

using namespace KamataEngine;

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
	delete player_;
	delete camera_;
	delete modelBlock_;

	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();
}

// 初期化
void GameScene::Initialize() {

	// 3dmodel生成
	model_ = KamataEngine::Model::Create();

	// モデルブロック作成
	model_ = KamataEngine::Model::Create();
	// ワールドトランスフォームの初期化
	worldtransform_.Initialize();

	// カメラ初期化
	camera_ = new Camera();
	camera_->Initialize();

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(1080, 720);

	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("mario.jpg");

	//==プレイヤー生成===//
	player_ = new Player();
	player_->initialize(model_, textureHandle_, camera_);

	// 要素数
	const uint32_t kBlockNumX = 20;
	// ブロック一個分の横幅
	const float kBlockWidth = 1.0f;
	//要素数を変更する
	worldTransformBlocks_.resize(kBlockNumX);

	//キューブの生成

}

// 更新
void GameScene::Update() {
	// デバッグカメラの更新

	debugCamera_->Update();

	// playerの更新
	player_->UpDate();
}

// 描画
void GameScene::Draw() {

	KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance();

	Model::PreDraw(dxcommon->GetCommandList());
	// model_->Draw(worldtransform_, debugCamera_->GetCamera(), textureHandle_);
	player_->Draw();
	Model::PostDraw();
}
