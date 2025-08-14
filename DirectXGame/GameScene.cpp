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
	modelBlock_ = KamataEngine::Model::Create();
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
	const float kBlockWidth = 2.0f;
	worldTransformBlocks_.resize(kBlockNumX);

	// キューブの生成
	for (uint32_t i = 0; i < kBlockNumX; ++i) {
		worldTransformBlocks_[i] = new WorldTransform();
		worldTransformBlocks_[i]->Initialize();
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		worldTransformBlocks_[i]->translation_.y = 0.0f;
	}
}

// 更新
void GameScene::Update() {
	// デバッグカメラの更新
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_C))
		isDebugCameraActive_ = true;
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_->TransferMatrix();
		
	} else {

		camera_->UpdateMatrix();
	}

	// playerの更新
	player_->UpDate();

	// ブロックの更新
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {

		worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
		// 定数バッファに転送する
		worldTransformBlock->TransferMatrix();
	}
}

// 描画
void GameScene::Draw() {

	KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance();

	Model::PreDraw(dxcommon->GetCommandList());
	// model_->Draw(worldtransform_, debugCamera_->GetCamera(), textureHandle_);
	player_->Draw();
	// ブロックの描画
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		modelBlock_->Draw(*worldTransformBlock, *camera_, nullptr);
	}
	Model::PostDraw();
}
