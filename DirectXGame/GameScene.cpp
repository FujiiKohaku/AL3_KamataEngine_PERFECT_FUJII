#include "GameScene.h"

using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {

	// 初期位置を確定
	pos_ = {0, 0, 0};
	// 3Dモデルデータの生成
	model_ = Model::CreateFromOBJ("player");
	// カメラの初期化
	camera_ = new Camera();
	camera_->Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, camera_, pos_);

	//------------------
	// スカイドーム
	//------------------

	modelSkydome_ = KamataEngine::Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, camera_);
}
// 更新
void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	skydome_->Update();
}
// 描画
void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 自キャラの描画
	model_->PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	skydome_->Draw();
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
	// カメラの解放
	delete camera_;
	// スカイドームモデルの解放
	delete modelSkydome_;
}