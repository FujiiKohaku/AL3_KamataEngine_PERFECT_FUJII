#include "GameScene.h"

using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");
	// 3Dモデルデータの生成
	model_ = Model::Create();
	camera_ = new Camera();
	// カメラの初期化
	camera_->Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, camera_);
	// 3Dモデルデータの生成(block)AL3_02_02
	modelBlock = Model::Create();
}
// 更新
void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
}
// 描画
void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 自キャラの描画
	model_->PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	model_->PostDraw();
}
// コンストラクタ
GameScene::GameScene() {}
// デストラクタ
GameScene::~GameScene() {
	// 3Dモデルデータの解散
	delete model_;
	// 自キャラの解散
	delete player_;
	/* 3Dモデルデータの解放(block)AL3_02_02*/
	for (WorldTransform* worldTransFormBlock : worldTransformBlocks_) {
		delete worldTransFormBlock;
	}
	worldTransformBlocks_.clear();
}