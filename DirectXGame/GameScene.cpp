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

	// 初期化AL3_02_02
	// 要素数AL3_02_02
	const uint32_t kNumBlockVirtial = mapChipField_->GetBlockWidth();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetBlockHeight();
	// ブロック１個文の横幅AL3_02_02
	//const float kBlockWidth = 2.0f;
	//const float kBlockHeight = 2.0f;

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	/// ここから02_03
	// skydomeを生成してモデルを渡して初期化
	//
	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, camera_);

	// マップチップをnewする
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GameScene::GenerateBlocks();
	{

		// 要素数を変更するAL3_02_02
		// 列数の設定(縦方向のブロック数)
		worldTransformBlocks_.resize(kNumBlockVirtial);
		for (uint32_t i = 0; i < kNumBlockVirtial; ++i) {
			// 一列の要素数を設定(縦方向のブロック数)
			worldTransformBlocks_[i].resize(kNumBlockHorizontal);
		}
		// キューブの生成AL3_02_02
		for (uint32_t i = 0; i < kNumBlockVirtial; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				/*if ((i + j) % 2 == 0) {
				    continue;
				}*/
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
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
			worldTransformBlock->matWorld_ = Function::MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

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
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送AL3_02_02*/
		camera_->TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送AL3_02_02*/

		camera_->UpdateMatrix();
		skydome_->Update();
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

			modelBlock->Draw(*worldTransformBlock, *camera_, nullptr);
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
	// AL3_02_04
	// マップチップフィールドの解放
	delete mapChipField_;
}


