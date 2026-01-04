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
	delete explanationSprite_;

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
	coinModel_ = Model::CreateFromOBJ("Coin");
	CreateCoinsFromMap();
	// トゲ生成
	CreateSpikesFromMap();
	// 敵生成
	CreateEnemiesFromMap();
	//------------------
	// プレイヤー関連
	//------------------
	pos_ = {2, 2, 0};
	model_ = Model::CreateFromOBJ("Alplayer");

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

	CameraController::GetInstance()->Initialize(camera_);
	CameraController::GetInstance()->SetTarget(player_);
	CameraController::GetInstance()->Reset();

	//-----------------
	// Sprite
	//-----------------
	textureHandleExp_ = TextureManager::Load("settingmoce.png");
	explanationSprite_ = Sprite::Create(textureHandleExp_, {0.0});
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
		CameraController::GetInstance()->Update();
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

#pragma region エネミーの処理まとめ
	// -----------------------
	// エネミー更新
	//------------------------
	for (auto* enemy : enemies_) {
		enemy->Update();
		if (enemy->IsDead()) {
			continue;
		}
		// ===============================
		//  吸い込み判定（前にある円）
		// ===============================
		if (player_->GetInhaleHitBox().active) {

			auto hb = player_->GetInhaleHitBox();

			Vector3 e = enemy->GetWorldTransform().translation_;
			Vector3 diff = e - hb.pos;

			//  距離を自前で計算
			float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

			if (dist < hb.radius) {
				enemy->StartPulled(player_);
			}
		}

		// ===============================
		//  体にぶつかったときの判定（今まで通り）
		// ===============================
		if (IsHitPlayerEnemy(player_, enemy)) {

			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}

	for (auto& bulletPtr : player_->GetBullets()) {

		if (!bulletPtr)
			continue; // 念のため

		auto& bullet = *bulletPtr; // 中身を参照で取り出す

		if (!bullet.IsAlive()) {
			continue;
		}
		for (auto* enemy : enemies_) {

			if (enemy->IsDead())
				continue;

			Vector3 diff = enemy->GetWorldTransform().translation_ - bullet.GetWorld().translation_;

			float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

			if (dist < enemy->GetRadius() + bullet.GetRadius()) {

				bullet.Kill();
				enemy->StartDying();  
				break;
			}
		}
	}

#pragma endregion

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

	// -----------------------
	// ImGuiデバッグ表示
	// -----------------------

	// ImGui::Begin("gamePlayScene Debug");
	// ImGui::Text("This is gamePlayScene!");
	// ImGui::End();
}

// 描画
void GameScene::Draw() {

	auto* dx = DirectXCommon::GetInstance();

	Model::PreDraw(dx->GetCommandList());

	// ---- 描画したいもの全部 ----
	player_->Draw();
	skydome_->Draw();

	for (auto& blockLine : worldTransformBlocks_) {
		for (auto* block : blockLine) {
			if (!block)
				continue;
			modelBlock_->Draw(*block, *camera_, nullptr);
		}
	}

	goal_->Draw(camera_);

	for (auto* coin : coins_) {
		if (IsNearPlayer(coin->GetPosition(), 20.0f)) {
			coin->Draw(camera_);
		}
	}

	for (auto* spike : spikes_) {
		spike->Draw(camera_);
	}

	for (auto* enemy : enemies_) {
		enemy->Draw(camera_);
	}

	Model::PostDraw();

	fade_.Draw();

	Sprite::PreDraw(dx->GetCommandList());
	// Sprite
	explanationSprite_->Draw();

	Sprite::PostDraw();
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
				coin->Initialize(coinModel_, pos);

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

			if (mapChipField_->GetMapChipTypeByIndex(j, i) != MapChipType::kEnemy) {
				continue;
			}

			Vector3 pos = mapChipField_->GetMapChipPositionbyIndex(j, i);

			EnemyBase* enemy = new WalkEnemy();
			enemy->Initialize(Model::CreateFromOBJ("CircleEnemy"), pos);

			enemies_.push_back(enemy);
		}
	}
}
#pragma region コイン処理
void GameScene::UpdateCoins() {

	for (auto* coin : coins_) {

		coin->Update();
		// 吸い込み
		if (player_->GetInhaleHitBox().active) {

			auto hb = player_->GetInhaleHitBox();

			Vector3 c = coin->GetWorldTransform().translation_;
			Vector3 diff = c - hb.pos;

			float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

			if (dist < hb.radius) {
				coin->StartPulled(player_);
			}
		}
		// 直ぶつかり
		if (coin->CheckCollision(player_)) {
			coin->Collect();            // ← 直接取得！
			player_->OnCollision(coin); // スコア加算だけ
		}
	}
}
#pragma endregion

bool GameScene::IsNearPlayer(const Vector3& pos, float range) {
	Vector3 p = player_->GetWorldTransform().translation_;
	float dx = pos.x - p.x;
	float dz = pos.z - p.z;
	return (dx * dx + dz * dz) <= (range * range);
}

// playerとEnemyの当たり判定
bool GameScene::IsHitPlayerEnemy(Player* player, EnemyBase* enemy) {
	Vector3 p = player->GetWorldTransform().translation_;
	Vector3 e = enemy->GetWorldTransform().translation_;

	Vector3 diff = p - e;
	float dist = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

	float r = player->GetRadius() + enemy->GetRadius();

	return dist < r;
}
