#include "GameScene.h"
using namespace KamataEngine;

//--------------------------------------------------
// デストラクタ
//--------------------------------------------------
GameScene::~GameScene() {
	// ====== オブジェクト解放 ======
	delete player_;
	delete skydome_;

	delete model_;
	delete modelBlock_;
	delete skydomeModel_;
	delete enemyModel_;
	delete dethParticleModel;

	delete debugCamera_;
	delete camera_;
	delete mapChipField_;
	delete cController_;

	delete deathParticles_;

	// Ready-Go モデル解放
	delete readyModel_;
	delete goModel_;

	// ====== 敵の解放 ======
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	// ====== ブロックの解放 ======
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
	// モデル生成
	model_ = Model::CreateFromOBJ("playermax", true);
	modelRolling = Model::CreateFromOBJ("roll", true);
	modelBlock_ = Model::CreateFromOBJ("block", true);
	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	enemyModel_ = Model::CreateFromOBJ("enemyBody", true);
	dethParticleModel = Model::CreateFromOBJ("deathParticle");

	// Ready/Go モデル
	readyModel_ = Model::CreateFromOBJ("rReady", true);
	goModel_ = Model::CreateFromOBJ("Go!", true);

// Ready モデル
	worldTransformReady_.Initialize();
	worldTransformReady_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformReady_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformReady_.translation_.z = 30.0f;     // カメラの正面
	worldTransformReady_.translation_.x = 2.5f;
	worldTransformReady_.translation_.y = 0.0f;       // 少し上に表示したければ調整
	worldTransformReady_.scale_ = {9.0f, 9.0f, 9.0f}; // 見やすいサイズに拡大

	// Go モデル
	worldTransformGo_.Initialize();
	worldTransformGo_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformGo_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformGo_.translation_.z = 30.0f;
	worldTransformGo_.translation_.x = 4.0f;
	worldTransformGo_.translation_.y = 0.0f;
	worldTransformGo_.scale_ = {9.0f, 9.0f, 9.0f};
	// ワールドトランスフォーム
	worldtransform_.Initialize();

	// カメラ
	camera_ = new Camera();
	camera_->Initialize();
	debugCamera_ = new DebugCamera(1080, 720);

	// テクスチャ
	textureHandle_ = TextureManager::Load("mario.jpg");

	// マップ
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks2.csv");

	// プレイヤー生成
	Vector3 playerposition = mapChipField_->GetMapChipPositionByIndex(10, 10);
	player_ = new Player();
	player_->Initialize(model_, modelRolling, camera_, playerposition);
	player_->SetMapChipField(mapChipField_);

	// ブロック生成
	GenerateBlocks();

	// スカイドーム生成
	skydome_ = new Skydome();
	skydome_->initialize(skydomeModel_, camera_);

	// カメラコントローラー
	cController_ = new CameraController();
	cController_->Initialize(camera_);
	cController_->SetTarget(player_);
	cController_->Reset();

	CameraController::Rect cameraArea = {12.0f, 200 - 12.0f, 6.0f, 6.0f};
	cController_->SetMovableArea(cameraArea);

	// 敵生成
	for (int32_t i = 0; i < 2; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14 + i * 2, 18);
		newEnemy->Initialize(enemyModel_, camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// デスパーティクル
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(dethParticleModel, camera_, playerposition);

	// フェーズ初期化
	phase_ = Phase::kFadeIn;
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// Ready-Go タイマー
	readyTimer_ = 0.0f;
}

//==================================================
// 更新
//==================================================
void GameScene::Update() {
	ChangePhese();
	WorldTransformUpdate(worldTransformReady_);
	WorldTransformUpdate(worldTransformGo_);
	switch (phase_) {
	case Phase::kFadeIn: {
		if (fade_) {
			fade_->Update();
			if (fade_->IsFinished()) {
				phase_ = Phase::kReady; // ← Ready演出へ
				readyTimer_ = 0.0f;
			}
		}
		skydome_->UpDate();
		cController_->Update();
	} break;

case Phase::kReady: {
		readyTimer_ += 1.0f / 60.0f;

		// READY 表示中（0〜1秒）
		if (readyTimer_ < 1.0f) {
			float t = readyTimer_;          // 0.0〜1.0
			float scale = 12.0f - 3.0f * t; // 最初は大きく(12)、徐々に小さく(9)
			worldTransformReady_.scale_ = {scale, scale, scale};
		}
		// GO!! 表示中（1.0〜1.5秒）
		else {
			float t = (readyTimer_ - 1.0f) * 2.0f * 3.14159f; // サイン波で揺らす
			float scale = 9.0f + 1.0f * sinf(t * 4.0f);       // 9を基準に拡縮
			worldTransformGo_.scale_ = {scale, scale, scale};
		}

		// 1.5秒経過でプレイ開始
		if (readyTimer_ > 1.5f) {
			phase_ = Phase::kPlay;
		}

		skydome_->UpDate();
		cController_->Update();
	} break;


	case Phase::kPlay: {
		skydome_->UpDate();
		cController_->Update();
		player_->Update();
		for (Enemy* enemy : enemies_)
			enemy->UpDate();

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
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
		}

		for (auto& line : worldTransformBlocks_) {
			for (auto& block : line) {
				if (!block)
					continue;
				WorldTransformUpdate(*block);
			}
		}
		CheckAllCollisions();
	} break;

	case Phase::kDeath: {
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			if (fade_)
				fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		skydome_->UpDate();
		cController_->Update();
		for (Enemy* enemy : enemies_)
			enemy->UpDate();
		if (deathParticles_)
			deathParticles_->Update();
	} break;

	case Phase::kFadeOut: {
		if (fade_) {
			fade_->Update();
			if (fade_->IsFinished())
				finished_ = true;
		}
		skydome_->UpDate();
		cController_->Update();
		for (Enemy* enemy : enemies_)
			enemy->UpDate();
	} break;
	}
}

//==================================================
// 描画
//==================================================
void GameScene::Draw() {
	KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	if (!player_->IsDead())
		player_->Draw();
	for (Enemy* enemy : enemies_)
		enemy->Draw();
	skydome_->Draw();

	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block)
				continue;
			modelBlock_->Draw(*block, *camera_, nullptr);
		}
	}

	if (deathParticles_)
		deathParticles_->Draw();

	// Ready-Go モデル描画
	if (phase_ == Phase::kReady) {
		if (readyTimer_ < 1.0f) {
			readyModel_->Draw(worldTransformReady_, *camera_);
		} else {
			goModel_->Draw(worldTransformGo_, *camera_);
		}
	}
	//readyModel_->Draw(worldTransformReady_, *camera_);
	fade_->Draw();
	Model::PostDraw();
}

//==================================================
// ブロック生成
//==================================================
void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
			}
		}
	}
}

//==================================================
// 衝突判定
//==================================================
void GameScene::CheckAllCollisions() {
	AABB aabb1 = player_->GetAABB();
	AABB aabb2;
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}

//==================================================
// フェーズ切替
//==================================================
void GameScene::ChangePhese() {
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			phase_ = Phase::kDeath;
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(dethParticleModel, camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		break;
	}
}
