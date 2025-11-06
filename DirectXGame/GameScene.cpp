#include "GameScene.h"
using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete camera_;
	delete modelSkydome_;
	delete debugCamera_;
	delete modelBlock_;
	delete mapChipField_;
	delete cController_;

	for (auto* coin : coins_) {
		delete coin;
	}
	coins_.clear();
	// ブロック（2次元配列）の解放
	for (auto& line : worldTransformBlocks_) {
		for (auto* block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}

// 初期化
void GameScene::Initialize() {

	//------------------
	// マップチップフィールド
	//------------------
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/map/blocks.csv");

	//------------------
	// ブロック配列の生成（要素サイズだけ確保）
	//------------------
	GenerateBlocks();

	// ブロックモデルのロード
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// ブロック生成
	CreateBlocksFromMap();
	// ゴール生成
	CreateGoalFromMap();
	// コイン生成
	CreateCoinsFromMap();
	//------------------
	// プレイヤー関連
	//------------------
	pos_ = {2, 2, 0};
	model_ = Model::CreateFromOBJ("player");

	camera_ = new Camera();
	camera_->Initialize();

	player_ = new Player();
	player_->Initialize(model_, camera_, pos_);
	player_->SetMapChipField(mapChipField_);
	//------------------
	// スカイドーム
	//------------------
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, camera_);

	//------------------
	// デバッグカメラ
	//------------------
	debugCamera_ = new DebugCamera(1280, 720);

	//------------------
	// カメラコントローラー
	//------------------

	cController_ = new CameraController();
	cController_->Initialize(camera_);
	cController_->SetTarget(player_);
	cController_->Reset();
}

// 更新
void GameScene::Update() {
	player_->Update();
	skydome_->Update();

	// -----------------------
	// ブロック更新
	// -----------------------
	for (auto& blockLine : worldTransformBlocks_) {
		for (auto* block : blockLine) {

			if (!block)
				continue; // ← nullチェック

			WorldTransformUpdate(*block);
		}
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		camera_->TransferMatrix();
	} else {
		camera_->UpdateMatrix();
		// -----------------------
		// カメラコントローラー更新
		// -----------------------
		cController_->Update();
	}

	// -----------------------
	// ゴール更新
	// -----------------------
	goal_->Update();

	// -----------------------
	// コイン更新
	// -----------------------
	for (auto* coin : coins_) {
		coin->Update();
		if (coin->CheckCollision(player_)) {
			coin->SetCollected(true);
		}
	}

	// ゴール判定ゴールは固定なのでplayerを渡すだけで平気
	if (goal_->CheckCollision(player_)) {
		finished_ = true;
	}
	// 死亡チェック
	if (player_->IsDead()) {
		finished_ = true; // GameOverSceneへ遷移
	}
	ImGui::Begin("gamePlayScene Debug");
	ImGui::Text("This is gamePlayScene!");
	ImGui::End();
}

// 描画
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	model_->PreDraw(dxCommon->GetCommandList());

	player_->Draw();
	skydome_->Draw();

	// -----------------------
	// ブロック描画（安全版）
	// -----------------------
	for (auto& blockLine : worldTransformBlocks_) {
		for (auto* block : blockLine) {
			if (!block)
				continue;
			modelBlock_->Draw(*block, *camera_, nullptr);
		}
	}
	// ゴール描画
	goal_->Draw(camera_);

	// コイン描画
	for (auto* coin : coins_) {
		coin->Draw(camera_);
	}
	model_->PostDraw();
}

// ===============================================
// ブロック配列（worldTransformBlocks_）の要素数を設定
// ===============================================
void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
}

// マップからブロックを生成
void GameScene::CreateBlocksFromMap() {
	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetMapChipPositionbyIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
			} else {
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}
}

// マップからゴールを生成
void GameScene::CreateGoalFromMap() {
	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kGoal) {
				Vector3 goalPos = mapChipField_->GetMapChipPositionbyIndex(j, i);
				goal_ = new Goal();
				goal_->Initialize(Model::CreateFromOBJ("player"), goalPos);
				return; // ゴールは1つだけならすぐ抜ける
			}
		}
	}
}
// マップからコインを生成
void GameScene::CreateCoinsFromMap() {

	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kCoin) {
				Vector3 pos = mapChipField_->GetMapChipPositionbyIndex(j, i);

				Coin* coin = new Coin();
				coin->Initialize(Model::CreateFromOBJ("player"), pos);

				coins_.push_back(coin);
			}
		}
	}
}
