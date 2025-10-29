#include "GameScene.h"

using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {

	// ブロック生成
	GenerateBlocks();
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
	//------------------
	// デバックカメラ
	//------------------
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	//------------------
	// ブロック
	//------------------

	modelBlock_ = Model::CreateFromOBJ("cube", true);

	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {

			// マップチップがブロックなら生成、それ以外はスキップ
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				// ブロック生成
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionbyIndex(j, i);
			}
		}
	}

	//------------------
	// マップチップフィールド
	//------------------
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
}
// 更新
void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	skydome_->Update();
	// ブロックの更新
	// ブロックの更新
	for (std::vector<KamataEngine::WorldTransform*>& blockLine : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* block : blockLine) {

			// スケール・回転・平行移動からアフィン変換行列を作成
			block->matWorld_ = MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);

			// 行列を定数バッファへ転送（GPUに送る）
			block->TransferMatrix();
		}
	}

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送AL3_02_02*/
		camera_->TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送AL3_02_02*/

		camera_->UpdateMatrix();
	}
}
// 描画
void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 自キャラの描画
	model_->PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<KamataEngine::WorldTransform*>& blockLine : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* block : blockLine) {
			modelBlock_->Draw(*block, *camera_, nullptr);
		}
	}

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
	// デバックカメラを開放
	delete debugCamera_;
	// ブロックの解放
	delete modelBlock_;
	// マップチップフィールドの解放
	delete mapChipField_;
	// ブロック（2次元配列）の解放
	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
	worldTransformBlocks_.clear();
}

// ===============================================
// ブロック配列（worldTransformBlocks_）の要素数を設定
// -----------------------------------------------
// ・MapChipFieldから取得したブロック数（縦×横）を使って
//   二次元配列をリサイズする
// ・ここではまだ中身（WorldTransform本体）は生成しない
// ===============================================
void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 外側vectorを縦方向（行）にリサイズ
	worldTransformBlocks_.resize(numBlockVirtical);

	// 各行の中を横方向（列）にリサイズ
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
}
