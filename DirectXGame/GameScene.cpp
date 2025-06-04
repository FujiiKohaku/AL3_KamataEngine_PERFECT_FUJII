#include "GameScene.h"
#include "Math.h"
using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	// ファイル名を指定してテクスチャを読み込む
	// textureHandle_ = TextureManager::Load("player.png");
	// 3Dモデルデータの生成
	model_ = Model::Create();
	model_ = Model::CreateFromOBJ("player", true);
	// カメラの初期化
	camera_.Initialize();
	// マップチップをnewするマップチップフィールドの初期化
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, &camera_, playerPosition);
	// 3Dモデルデータの生成(block)AL3_02_02
	modelBlock = Model::CreateFromOBJ("block", true);
	// プレイヤーがマップチップの構造を知って、地形に応じた処理ができるようにするAL3_02_07
	player_->SetMapChipField(mapChipField_);

	// ワールドトランスフォームの初期化
	//
	// 02_06カメラコントローラ
	cController_ = new CameraController(); // 生成
	cController_->Initialize(&camera_);    // 初期化
	cController_->SetTarget(player_);      // 02_06
	cController_->Reset();                 // 02_06

	// 初期化AL3_02_02

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	/// ここから02_03
	// skydomeを生成してモデルを渡して初期化
	//
	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();
	// Al2_02_06

	// 02_06カメラコントローラ スライド18枚目
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cController_->SetMoveableArea(cameraArea);
}

// 更新
void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	/* ブロックの更新AL3_02_02*/
	for (const std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	// デバックの時Cキーを押すと状態が反転する
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive = !isDebugCameraActive;
	}
#endif // ! _DEBUG
	// カメラの処理AL3_02_02*/
	if (isDebugCameraActive) {
		// デバッグカメラの更新AL3_02_02*/
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送AL3_02_02*/
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送AL3_02_02*/

		camera_.UpdateMatrix();
		skydome_->Update();
		cController_->Upadate();
	}
}
// 描画
void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 自キャラの描画
	model_->PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	// ブロックの描画AL3_02_02
	for (const std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock->Draw(*worldTransformBlock, camera_, nullptr);
		}
	}
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
	/* 3Dモデルデータの解放(block)AL3_02_02*/
	for (std::vector<WorldTransform*>& worldTransformBkockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBkockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	// デバッグカメラの解放
	delete debugCamera_;
	// AL3_02_03
	delete skydome_;
	// マップチップフィールドデリーと
	delete mapChipField_;
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetBlockHeight();
	uint32_t numBlockHorizontal = mapChipField_->GetBlockWidth();

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
