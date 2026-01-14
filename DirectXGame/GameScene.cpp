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
	delete tutorialSignModel_;
	for (Coin* coin : coins_) {
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
	for (JumpHopper* h : jumpHoppers_) {
		delete h;
	}
	jumpHoppers_.clear();

	enemies_.clear();
	worldTransformBlocks_.clear();
}

// 初期化
void GameScene::Initialize() {
	//switch (stageState_) {
	//case StageState::Tutorial:
	//	//チュートリアル看板
	//	tutorialSignModel_ = Model::CreateFromOBJ("kanban", true);
	//	worldTransformTutorialSign_.Initialize();

	//	break;
	//case StageState::Stage1:
	//	break;
	//case StageState::Stage2:
	//	break;
	//default:
	//	break;
	//}
	tutorialSignModel_ = Model::CreateFromOBJ("kanban", true);
	worldTransformTutorialSign_.Initialize();
	worldTransformTutorialSign_.translation_ = {5.0f, 0.0f, 5.0f};
	worldTransformTutorialSign_.rotation_.y = -std::numbers::pi_v<float>/2.0f;
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

	//------------------
	// プレイヤー関連
	//------------------
	pos_ = {2, 2, 0};
	model_ = Model::CreateFromOBJ("AlPlayer");

	camera_ = new Camera();
	camera_->Initialize();

	player_ = new Player();
	player_->Initialize(model_, camera_, pos_);
	player_->SetMapChipField(mapChipField_);

	// 敵生成
	CreateEnemiesFromMap();
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

	// ジャンプ台
	jumpHopperModel_ = Model::CreateFromOBJ("spring", true);
	CreateJumpHoppersFromMap();

	textureFullHp_ = TextureManager::Load("Full.png");
	textureEmptyHp_ = TextureManager::Load("empty.png");

	for (int i = 0; i < 3; i++) {
		heartsFull_.push_back(Sprite::Create(textureFullHp_, {0, 0}));
		heartsEmpty_.push_back(Sprite::Create(textureEmptyHp_, {0, 0}));
	}

	BgmHandle_ = Audio::GetInstance()->LoadWave("gameScene.mp3");
	bgmPlayHandle_ = Audio::GetInstance()->PlayWave(BgmHandle_, true, 0.5f);

	AttackSEHandle_ = Audio::GetInstance()->LoadWave("Attack.mp3");
	coinSEHandle_ = Audio::GetInstance()->LoadWave("coin.mp3");
}

// 更新
void GameScene::Update() {

	switch (stageState_) {
	case StageState::Tutorial:

		break;
	case StageState::Stage1:
		break;
	case StageState::Stage2:
		break;
	default:
		break;
	}
	player_->Update();
	skydome_->Update();
	fade_.Update();

	// -----------------------
	// ジャンプホッパー更新
	// -----------------------
	for (JumpHopper* h : jumpHoppers_) {
		h->Update(player_);
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
		if (!player_->IsDead() && IsHitPlayerEnemy(player_, enemy)) {

			if (player_->IsDead()) {
				return;
			}
			if (player_->IsInvincible())
				return;

			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
			Audio::GetInstance()->PlayWave(AttackSEHandle_);
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
		Audio::GetInstance()->StopWave(bgmPlayHandle_);
	}

	// -----------------------
	// プレイヤー死亡チェック
	// -----------------------
	if (player_->IsDead()) {
		isGameOver_ = true;
		Audio::GetInstance()->StopWave(bgmPlayHandle_);
	}
	// -----------------------
	// プレイヤークリアチェック
	// -----------------------

	if (player_->IsGoal()) {
		finished_ = true;
		Audio::GetInstance()->StopWave(bgmPlayHandle_);
	}

	// -----------------------
	// ImGuiデバッグ表示
	// -----------------------

	// ImGui::Begin("gamePlayScene Debug");
	// ImGui::Text("This is gamePlayScene!");
	// ImGui::End();

	//	チュートリアル看板
	WorldTransformUpdate(worldTransformTutorialSign_);
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
	tutorialSignModel_->Draw(worldTransformTutorialSign_, *camera_, nullptr);
	//switch (stageState_) {
	//case StageState::Tutorial:
	//	
	//	break;
	//case StageState::Stage1:
	//	break;
	//case StageState::Stage2:
	//	break;
	//default:
	//	break;
	//}
	goal_->Draw(camera_);
	// ホッパー
	for (auto* h : jumpHoppers_) {
		h->Draw();
	}
	for (auto* coin : coins_) {
		if (IsNearPlayer(coin->GetPosition(), 15.0f)) {
			coin->Draw(camera_);
		}
	}

	for (auto* spike : spikes_) {
		spike->Draw(camera_);
	}

	for (auto* enemy : enemies_) {
		enemy->Draw(camera_);
	}
	player_->GetInhaleEffect()->Draw(camera_);
	Model::PostDraw();

	fade_.Draw();

	Sprite::PreDraw(dx->GetCommandList());
	// Sprite
	explanationSprite_->Draw();

	int hp = player_->GetHp();
	int maxHp = 3; // 後で変数にする

	// 無敵なら点滅タイミングを計算
	bool flash = false;
	if (player_->IsInvincible()) {
		float t = player_->GetInvincibleTimer();
		flash = (fmodf(t, 0.2f) < 0.1f);
	}

	for (int i = 0; i < maxHp; i++) {

		Vector2 pos(20.0f + i * 80.0f, 20.0f);

		if (i < hp) {
			// 残ってるハート（点滅しない）
			heartsFull_[i]->SetPosition(pos);
			heartsFull_[i]->Draw();
		} else {

			// 減ったハート（ここだけ点滅）
			if (flash)
				continue;

			heartsEmpty_[i]->SetPosition(pos);
			heartsEmpty_[i]->Draw();
		}
	}

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
				WorldTransformUpdate(*worldTransform);
			} else {
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}
}
// マップからジャンプ台を生成
void GameScene::CreateJumpHoppersFromMap() {

	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtical(); y++) {
		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizontal(); x++) {

			if (mapChipField_->GetMapChipTypeByIndex(x, y) == MapChipType::kJumpPad) {

				Vector3 pos = mapChipField_->GetMapChipPositionbyIndex(x, y);

				JumpHopper* hopper = new JumpHopper();
				hopper->Initialize(jumpHopperModel_, camera_, pos, 0.5f);

				jumpHoppers_.push_back(hopper);
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

void GameScene::CreateEnemiesFromMap() {

	for (uint32_t i = 0; i < mapChipField_->GetNumBlockVirtical(); ++i) {
		for (uint32_t j = 0; j < mapChipField_->GetNumBlockHorizontal(); ++j) {

			auto type = mapChipField_->GetMapChipTypeByIndex(j, i);
			Vector3 pos = mapChipField_->GetMapChipPositionbyIndex(j, i);

			switch (type) {

			case MapChipType::kEnemy: {
				auto enemy = new WalkEnemy();
				enemy->Initialize(Model::CreateFromOBJ("CircleEnemy"), pos);
				enemies_.push_back(enemy);
				break;
			}

			case MapChipType::kEnemyFlyer: {
				auto enemy = new EnemyFlyer();
				enemy->Initialize(Model::CreateFromOBJ("CircleEnemy"), pos);
				enemies_.push_back(enemy);
				break;
			}

			case MapChipType::kEnemyChaser: {
				auto enemy = new ChaserEnemy();
				enemy->Initialize(Model::CreateFromOBJ("CircleEnemy"), pos, player_);
				enemies_.push_back(enemy);
				break;
			}

			default:
				break;
			}
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
			Audio::GetInstance()->PlayWave(coinSEHandle_);
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
