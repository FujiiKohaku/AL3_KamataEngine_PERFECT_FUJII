#include "GameScene.h"

using namespace KamataEngine;

//--------------------------------------------------
// デストラクタ
//--------------------------------------------------
GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
	delete player_;
	delete camera_;
	delete modelBlock_;
	delete skydomeModel_;
	delete mapChipField_;
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();
}

//--------------------------------------------------
// 初期化
//--------------------------------------------------
void GameScene::Initialize() {

	// 3Dモデル生成
	model_ = KamataEngine::Model::Create();
	modelBlock_ = KamataEngine::Model::Create();

	// ワールドトランスフォーム初期化
	worldtransform_.Initialize();

	// カメラ初期化
	camera_ = new Camera();
	camera_->Initialize();

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(1080, 720);

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("mario.jpg");

	// プレイヤー生成
	player_ = new Player();
	player_->initialize(model_, textureHandle_, camera_);

	// ブロック列の準備
	const uint32_t kBlockNumX = 20; // 要素数
	const float kBlockWidth = 2.0f; // ブロック1個分の横幅
	worldTransformBlocks_.resize(kBlockNumX);

	// キューブ（ブロック）の生成・配置
	for (uint32_t i = 0; i < kBlockNumX; ++i) {
		worldTransformBlocks_[i] = new WorldTransform();
		worldTransformBlocks_[i]->Initialize();
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		worldTransformBlocks_[i]->translation_.y = 0.0f;
	}

	// スカイドーム
	skydome_ = new Skydome();
	skydomeModel_ = Model::CreateFromOBJ("skydome", true); // skydomeModel_ が指しているモデルに対して、Model クラス版の CreateFromOBJ を呼ぶ
	skydome_->initialize(skydomeModel_, camera_);

	// マップチップフィールドの初期化
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipData("Resource/Blocks.csv");
}

//--------------------------------------------------
// 更新
//--------------------------------------------------
void GameScene::Update() {

	// デバッグカメラ切替（Debug時のみ）
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = true;
	}
#endif

	// カメラ更新
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		camera_->TransferMatrix(); // ビュープロジェクション行列の転送
	} else {
		camera_->UpdateMatrix();
	}

	// プレイヤー更新
	player_->UpDate();

	// ブロック更新（ワールド行列→定数バッファ転送）
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
		worldTransformBlock->TransferMatrix();
	}

	// スカイドーム更新
	skydome_->UpDate();
}

//--------------------------------------------------
// 描画
//--------------------------------------------------
void GameScene::Draw() {

	KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance();

	Model::PreDraw(dxcommon->GetCommandList());

	// モデル描画（プレイヤー自身で描画）
	// model_->Draw(worldtransform_, debugCamera_->GetCamera(), textureHandle_);
	player_->Draw();

	// スカイドーム描画
	skydome_->Draw();

	// ブロック描画
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		modelBlock_->Draw(*worldTransformBlock, *camera_, nullptr);
	}

	Model::PostDraw();
}
