#include "GameScene.h"

using namespace KamataEngine;

//--------------------------------------------------
// デストラクタ
//--------------------------------------------------
GameScene::~GameScene() {
	delete player_;  // 先にプレイヤーを消す
	delete skydome_; // 先にスカイドームを消す

	delete model_;
	delete modelBlock_;
	delete skydomeModel_;

	delete debugCamera_;
	delete camera_;
	delete mapChipField_;

	delete cController_;
	delete enemy_;

	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

//==================================================
// 初期化
//==================================================
void GameScene::Initialize() {

	//--------------------------------------------------
	// モデル生成
	//--------------------------------------------------
	model_ = Model::CreateFromOBJ("playermax", true);      // プレイヤーモデル
	modelBlock_ = Model::CreateFromOBJ("block", true);     // ブロックモデル
	skydomeModel_ = Model::CreateFromOBJ("skydome", true); // スカイドームモデル
	enemyModel_ = Model::CreateFromOBJ("enemy", true);     // エネミーモデル

	//--------------------------------------------------
	// ワールドトランスフォーム
	//--------------------------------------------------
	worldtransform_.Initialize();

	//--------------------------------------------------
	// カメラ関連
	//--------------------------------------------------
	camera_ = new Camera();
	camera_->Initialize();
	debugCamera_ = new DebugCamera(1080, 720);

	//--------------------------------------------------
	// テクスチャ読み込み
	//--------------------------------------------------
	textureHandle_ = TextureManager::Load("mario.jpg");

	//--------------------------------------------------
	// マップチップフィールド
	//--------------------------------------------------
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	//--------------------------------------------------
	// プレイヤー生成
	//--------------------------------------------------
	Vector3 playerposition = mapChipField_->GetMapChipPositionByIndex(5, 5);
	player_ = new Player();
	player_->Initialize(model_, camera_, playerposition);
	player_->SetMapChipField(mapChipField_);

	//--------------------------------------------------
	// ブロック生成
	//--------------------------------------------------
	GenerateBlocks();

	//--------------------------------------------------
	// スカイドーム生成
	//--------------------------------------------------
	skydome_ = new Skydome();
	skydome_->initialize(skydomeModel_, camera_);

	//--------------------------------------------------
	// カメラコントローラー
	//--------------------------------------------------
	cController_ = new CameraController();
	cController_->Initialize(camera_);
	cController_->SetTarget(player_);
	cController_->Reset();

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cController_->SetMovableArea(cameraArea);

	//--------------------------------------------------
	// 敵生成
	//--------------------------------------------------
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10, 10);
	enemy_ = new Enemy();
	enemy_->Initialize(enemyModel_, camera_, enemyPosition);
}


//--------------------------------------------------
// 更新
//--------------------------------------------------
void GameScene::Update() {

	// デバッグカメラ切替（Debug時のみ）
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_; // ←トグルにする
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
		cController_->Update(); // デバッグカメラのときは呼ばない
	}
	
	// プレイヤー更新
	player_->Update();
	// エネミー更新
	enemy_->UpDate();
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;

			// アフィン変換～DirectXに転送
			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	skydome_->UpDate(); // スカイドーム更新


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
	enemy_->Draw();

	// スカイドーム描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, *camera_, nullptr);
		}
	}
	Model::PostDraw();
}

#pragma region ブロック生成関数
// ブロック生成関数
void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {

		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
#pragma endregion