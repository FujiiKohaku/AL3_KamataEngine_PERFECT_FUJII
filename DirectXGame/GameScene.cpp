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
	// ====== 敵の解放 ======
	for (Enemy2* enemy : enemies2_) {
		delete enemy;
	}
	enemies_.clear();
	// ====== ヒットエフェクト解放 ======
	for (HitEffect* effect : hitEffects_) {
		delete effect;
	}
	hitEffects_.clear();

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
	modelBlock_ = Model::CreateFromOBJ("tileBlock", true);
	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	enemyModel_ = Model::CreateFromOBJ("enemyBody", true);
	dethParticleModel = Model::CreateFromOBJ("deathParticle");
	spikeModel_ = Model::CreateFromOBJ("spike", true);
	// Ready/Go モデル
	readyModel_ = Model::CreateFromOBJ("rReady", true);
	goModel_ = Model::CreateFromOBJ("Go!", true);
	enemyModel2_ = Model::CreateFromOBJ("enemyBody", true);
	// カメラ
	camera_ = new Camera();
	camera_->Initialize();
	debugCamera_ = new DebugCamera(1080, 720);

	// ===== ヒットエフェクトにモデルとカメラを渡す =====
	HitEffect::SetModel(Model::CreateFromOBJ("AttackEffect", true));
	HitEffect::SetCamera(camera_);

	springModel_ = Model::CreateFromOBJ("spring", true);

	fireModel_ = Model::CreateFromOBJ("fire", true);
	// Ready モデル
	worldTransformReady_.Initialize();
	worldTransformReady_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformReady_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformReady_.translation_.z = 30.0f; // カメラの正面
	worldTransformReady_.translation_.x = 2.5f;
	worldTransformReady_.translation_.y = 0.0f;
	worldTransformReady_.scale_ = {9.0f, 9.0f, 9.0f};

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

	// テクスチャ
	textureHandle_ = TextureManager::Load("mario.jpg");

	// マップ
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks3.csv");

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
				phase_ = Phase::kReady;
				readyTimer_ = 0.0f;
			}
		}
		skydome_->UpDate();
		cController_->Update();
	} break;

	case Phase::kReady: {
		readyTimer_ += 1.0f / 60.0f;
		if (readyTimer_ < 1.0f) {
			float t = readyTimer_;
			float scale = 12.0f - 3.0f * t;
			worldTransformReady_.scale_ = {scale, scale, scale};
		} else {
			float t = (readyTimer_ - 1.0f) * 2.0f * 3.14159f;
			float scale = 9.0f + 1.0f * sinf(t * 4.0f);
			worldTransformGo_.scale_ = {scale, scale, scale};
		}
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
		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
		}
		for (Enemy2* enemy : enemies2_) {
			enemy->Update();
		}
		// ===== デバッグカメラ切り替え =====
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_G)) {
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

		// ===== ブロック更新 =====
		for (auto& line : worldTransformBlocks_) {
			for (auto& block : line) {
				if (!block)
					continue;
				WorldTransformUpdate(*block);
			}
		}

		// ===== スパイク更新 =====
		for (auto& line : worldTransformSpikes_) {
			for (auto& spike : line) {
				if (!spike)
					continue;
				WorldTransformUpdate(*spike);
			}
		}
		// ===== バネ更新 =====
		for (auto& line : worldTransformSprings_) {
			for (auto& spring : line) {
				if (!spring)
					continue;
				WorldTransformUpdate(*spring);
			}
		}
		// ===== 火の更新 =====
		static float t = 0.0f;
		t += 1.0f / 60.0f; // フレームごとに進める

		for (auto& line : worldTransformFires_) {
			for (auto& fire : line) {
				if (!fire)
					continue;

				float amplitude = 1.0f; // 上下の振れ幅
				float speed = 2.0f;     // 上下の速さ

				float baseY = fire->translation_.y; // 元の高さ（初期値を基準にしたいなら別で保持すると良い）
				fire->translation_.y = baseY + amplitude * sinf(t * speed);

				WorldTransformUpdate(*fire);
			}
		}
		player_->CheckSpringCollision(worldTransformSprings_);
		player_->CheckFireCollision(worldTransformFires_);
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

	// ====== フェーズ共通で敵削除処理 ======
	enemies_.remove_if([this](Enemy* enemy) {
		if (enemy->IsDead()) {
			// ヒットエフェクト生成
			hitEffects_.push_back(HitEffect::Create(enemy->GetWorldPosition()));
			delete enemy;
			return true;
		}
		return false;
	});

	// ====== ヒットエフェクト更新 ======
	for (auto it = hitEffects_.begin(); it != hitEffects_.end();) {
		(*it)->Update();
		if ((*it)->IsFinished()) {
			delete *it;
			it = hitEffects_.erase(it);
		} else {
			++it;
		}
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

// Enemy2 の描画
	for (Enemy2* enemy2 : enemies2_) {
		enemy2->Draw();
	}

	skydome_->Draw();

	// ====== ブロック描画 ======
	for (auto& line : worldTransformBlocks_) {
		for (auto& block : line) {
			if (!block)
				continue;
			modelBlock_->Draw(*block, *camera_, nullptr);
		}
	}

	// ====== スパイク描画 ======
	for (auto& line : worldTransformSpikes_) {
		for (auto& spike : line) {
			if (!spike)
				continue;
			spikeModel_->Draw(*spike, *camera_, nullptr);
		}
	}

	// ====== バネ描画 ======
	for (auto& line : worldTransformSprings_) {
		for (auto& spring : line) {
			if (!spring)
				continue;
			springModel_->Draw(*spring, *camera_, nullptr);
		}
	}

	// ====== 動く火の描画 ======
	for (auto& line : worldTransformFires_) {
		for (auto& fire : line) {
			if (!fire)
				continue;
			fireModel_->Draw(*fire, *camera_, nullptr);
		}
	}

	if (deathParticles_)
		deathParticles_->Draw();

	// ====== ヒットエフェクト描画 ======
	for (auto* effect : hitEffects_) {
		effect->Draw();
	}

	if (phase_ == Phase::kReady) {
		if (readyTimer_ < 1.0f) {
			readyModel_->Draw(worldTransformReady_, *camera_);
		} else {
			goModel_->Draw(worldTransformGo_, *camera_);
		}
	}
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
	worldTransformSpikes_.resize(numBlockVirtical);

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
		worldTransformSpikes_[i].resize(numBlockHorizontal);
	}

	worldTransformFires_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformFires_[i].resize(numBlockHorizontal);
	}
	worldTransformSprings_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformSprings_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(j, i);

			if (type == MapChipType::kBlock) {
				WorldTransform* wt = new WorldTransform();
				wt->Initialize();
				wt->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = wt;

			} else if (type == MapChipType::kSpike) {
				WorldTransform* wt = new WorldTransform();
				wt->Initialize();
				wt->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

				//  サイズを0.5倍
				wt->scale_ = {0.5f, 0.5f, 0.5f};

				// ★ 左に90°回転（Y軸方向を想定）
				wt->rotation_.y = -std::numbers::pi_v<float> / 2.0f;

				worldTransformSpikes_[i][j] = wt;

			} else if (type == MapChipType::kEnemy) {
				Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(j, i);
				Enemy* newEnemy = new Enemy();
				newEnemy->Initialize(enemyModel_, camera_, enemyPos, mapChipField_);
				enemies_.push_back(newEnemy);
			}

			else if (type == MapChipType::kSpring) {
				WorldTransform* wt = new WorldTransform();
				wt->Initialize();
				wt->scale_ = {0.5f, 0.5f, 0.5f};
				wt->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				// 配列に追加
				worldTransformSprings_[i][j] = wt;
			} else if (type == MapChipType::kMovingFire) {
				WorldTransform* wt = new WorldTransform();
				wt->Initialize();
				wt->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

				// 火の大きさは少し小さめに
				wt->scale_ = {0.5f, 0.5f, 0.5f};

				// 配列に追加（worldTransformFires_ を用意しておく）
				worldTransformFires_[i][j] = wt;
			} else if (type == MapChipType::kEnemy2) {
				Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(j, i);
				Enemy2* newEnemy2 = new Enemy2();
				newEnemy2->Initialize(enemyModel2_, camera_, enemyPos, mapChipField_);
				enemies2_.push_back(newEnemy2);
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

    // Enemy1 との当たり判定
    for (Enemy* enemy : enemies_) {
        aabb2 = enemy->GetAABB();
        if (IsCollision(aabb1, aabb2)) {
            player_->OnCollision(enemy);
            enemy->OnCollision(player_);
        }
    }

    // Enemy2 との当たり判定
    for (Enemy2* enemy2 : enemies2_) {
        aabb2 = enemy2->GetAABB();
        if (IsCollision(aabb1, aabb2)) {
            player_->OnCollision(enemy2);
            enemy2->OnCollision(player_);
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
