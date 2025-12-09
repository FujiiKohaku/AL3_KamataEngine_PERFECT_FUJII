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
	// スパイクの解放
	for (auto* spike : spikes_) {
		delete spike;
	}
	spikes_.clear();
	for (auto* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	worldTransformBlocks_.clear();
}

// 初期化
void GameScene::Initialize() {

	//------------------
	// マップチップフィールド
	//------------------
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv(mapCsvPath_);

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
	// トゲ生成
	CreateSpikesFromMap();
	// 敵生成
	CreateEnemiesFromMap();
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
	fade_.Update();
	// -----------------------
	// ブロック更新
	// -----------------------
	for (auto& blockLine : worldTransformBlocks_) {
		for (auto* block : blockLine) {

			if (!block)
				continue; //  nullチェック

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
	UpdateCoins();

	// -----------------------
	// スパイク更新
	// -----------------------
	for (auto* spike : spikes_) {
		spike->Update();
		if (spike->CheckCollision(player_)) {
			player_->OnCollision(spike);
		}
	}

	// -----------------------
	// エネミー更新
	// -----------------------
	for (auto* enemy : enemies_) {
		enemy->Update();
		if (enemy->CheckCollision(player_)) {
			enemy->OnCollision(player_);
			player_->OnCollision(enemy);
		}
	}
	// -----------------------
	// ゴール判定
	// -----------------------
	if (goal_->CheckCollision(player_)) {
		player_->OnCollision(goal_);
	}

	// -----------------------
	// プレイヤー死亡チェック
	// -----------------------
	if (player_->IsDead()) {
		isGameOver_ = true;
	}
	// -----------------------
	// プレイヤークリアチェック
	// -----------------------

	if (player_->IsGoal()) {
		finished_ = true;
	}

	// ImGui::Begin("gamePlayScene Debug");
	// ImGui::Text("This is gamePlayScene!");
	// ImGui::End();
}

// 描画
void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ========= プレイヤー&スカイドーム =========
	model_->PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	skydome_->Draw();

	// ========= ブロック =========
	modelBlock_->PreDraw(dxCommon->GetCommandList());
	for (auto& blockLine : worldTransformBlocks_) {
		for (auto* block : blockLine) {
			if (!block)
				continue;
			modelBlock_->Draw(*block, *camera_, nullptr);
		}
	}

	// ========= ゴール =========
	goal_->Draw(camera_);

	// ========= コイン =========
	Model::PreDraw(dxCommon->GetCommandList());

	for (auto* coin : coins_) {
		if (!IsNearPlayer(coin->GetPosition(), 25.0f))
			continue;
		coin->Draw(camera_);
	}

	Model::PostDraw();

	// ========= スパイク =========
	for (auto& spike : spikes_) {
		spike->Draw(camera_);
	}

	// ========= エネミー =========
	for (auto* enemy : enemies_) {
		enemy->Draw(camera_);
	}

	model_->PostDraw();

	fade_.Draw();
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
				goal_->Initialize(Model::CreateFromOBJ("goal"), goalPos);
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
				coin->Initialize(Model::CreateFromOBJ("Coin"), pos);

				coins_.push_back(coin);
			}
		}
	}
}
// マップからトゲを生成
void GameScene::CreateSpikesFromMap() {
	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSpike) {
				Vector3 pos = mapChipField_->GetMapChipPositionbyIndex(j, i);
				Spike* spike = new Spike();
				spike->Initialize(Model::CreateFromOBJ("spike"), pos);
				spikes_.push_back(spike);
			}
		}
	}
}
// マップからエネミーを生成
void GameScene::CreateEnemiesFromMap() {
	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kEnemy) {
				Vector3 pos = mapChipField_->GetMapChipPositionbyIndex(j, i);
				Enemy* enemy = new Enemy();
				enemy->Initialize(Model::CreateFromOBJ("enemy"), pos);
				enemies_.push_back(enemy);
			}
		}
	}
}
void GameScene::UpdateCoins() {

	for (auto* coin : coins_) {

		// 遠いコインは処理しない
		if (!IsNearPlayer(coin->GetPosition(), 20.0f))
			continue;

		coin->Update();

		if (coin->CheckCollision(player_)) {
			player_->OnCollision(coin);
		}
	}
}

bool GameScene::IsNearPlayer(const Vector3& pos, float range) {
	Vector3 p = player_->GetWorldTransform().translation_;
	float dx = pos.x - p.x;
	float dz = pos.z - p.z;
	return (dx * dx + dz * dz) <= (range * range);
}
