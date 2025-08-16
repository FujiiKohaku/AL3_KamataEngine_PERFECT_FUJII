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

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

//--------------------------------------------------
// 初期化
//--------------------------------------------------
void GameScene::Initialize() {

	// 3Dモデル生成
	model_ = KamataEngine::Model::Create();
	model_ = Model::CreateFromOBJ("player", true);
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

	// マップチップフィールドの初期化
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	Vector3 playerposition = mapChipField_->GetMapChipPositionByIndex(5, 5);
	// プレイヤー生成
	player_ = new Player();
	player_->Initialize(model_, camera_, playerposition);
	player_->SetMapChipField(mapChipField_);
	// ブロック列の準備
	// const uint32_t kBlockNumX = 20; // 要素数
	// const float kBlockWidth = 2.0f; // ブロック1個分の横幅
	/*worldTransformBlocks_.resize(kBlockNumX);*/
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// スカイドーム
	skydome_ = new Skydome();
	skydomeModel_ = Model::CreateFromOBJ("skydome", true); // skydomeModel_ が指しているモデルに対して、Model クラス版の CreateFromOBJ を呼ぶ
	skydome_->initialize(skydomeModel_, camera_);

	GenerateBlocks();
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
	player_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;

			// アフィン変換～DirectXに転送
			WorldTransformUpdate(*worldTransformBlock);
		}
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