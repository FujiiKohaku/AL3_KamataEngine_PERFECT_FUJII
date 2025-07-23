#include "GameScene.h"
#include "Math.h"
#include "TitleScene.h"

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene()
    : model_(nullptr), modelAttack_(nullptr), player_(nullptr), mapChipField_(nullptr), debugCamera_(nullptr), skydome_(nullptr), cController_(nullptr), modelSkydome_(nullptr), modelBlock(nullptr),
      enemy_model_(nullptr), deathParticleModel(nullptr), deathParticles_(nullptr), modelPlayerHit_(nullptr), fade_(nullptr), isDebugCameraActive(false), finished_(false), phase_(Phase::kFadeIn) {}

// デストラクタ
GameScene::~GameScene() {
	delete model_;
	delete modelAttack_;
	delete player_;

	for (std::vector<WorldTransform*>& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
	delete skydome_;
	delete mapChipField_;
	delete cController_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (HitEffect* effect : hitEffects_) {
		delete effect;
	}
	hitEffects_.clear();

	delete deathParticles_;
	delete deathParticleModel;
	delete fade_;
}

// 初期化
void GameScene::Initialize() {
	// マップとプレイヤー初期位置
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// モデル読み込み
	model_ = Model::CreateFromOBJ("player", true);
	modelAttack_ = Model::CreateFromOBJ("attack_effect", true);

	// プレイヤー生成
	player_ = new Player();
	player_->Initialize(model_, modelAttack_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// カメラ制御
	camera_.Initialize();

	cController_ = new CameraController();
	cController_->Initialize(&camera_);
	cController_->SetTarget(player_);
	cController_->Reset();

	// デバッグカメラ
	debugCamera_ = new DebugCamera(1280, 720);

	// スカイドーム
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// ブロック生成
	modelBlock = Model::CreateFromOBJ("block", true);
	GenerateBlocks();

	// カメラの移動範囲設定
	CameraController::Rect cameraArea = {12.0f, 88.0f, 6.0f, 6.0f};
	cController_->SetMoveableArea(cameraArea);

	// 敵の初期化
	enemy_model_ = Model::CreateFromOBJ("enemy", true);
	for (int i = 0; i < 2; ++i) {
		Enemy* enemy = new Enemy();
		Vector3 pos = mapChipField_->GetMapChipPositionByIndex(14 + i * 2, 18);
		enemy->Initialize(enemy_model_, &camera_, pos);
		//ここでセットゲームシーン
		enemy->SetGameScene(this);
		enemies_.push_back(enemy);
	}

	// デスパーティクルモデル
	deathParticleModel = Model::CreateFromOBJ("deathParticle", true);

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// ヒットエフェクト共有設定
	modelPlayerHit_ = Model::CreateFromOBJ("particle", true);
	HitEffect::SetModel(modelPlayerHit_);
	HitEffect::SetCamera(&camera_);
}

// ブロック生成処理
void GameScene::GenerateBlocks() {
	uint32_t height = mapChipField_->GetBlockHeight();
	uint32_t width = mapChipField_->GetBlockWidth();

	worldTransformBlocks_.resize(height);
	for (uint32_t i = 0; i < height; ++i) {
		worldTransformBlocks_[i].resize(width, nullptr);
		for (uint32_t j = 0; j < width; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* wt = new WorldTransform();
				wt->Initialize();
				wt->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = wt;
			}
		}
	}
}

// フェーズ遷移
void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			phase_ = Phase::kDeath;
			const Vector3& deathPos = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(deathParticleModel, &camera_, deathPos);
		}
		break;
	case Phase::kDeath:
		break;
	default:
		break;
	}
}

// 更新
void GameScene::Update() {
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kPlay;
		}
		[[fallthrough]];
	case Phase::kPlay:
	case Phase::kDeath:
	case Phase::kFadeOut:
		skydome_->Update();
		cController_->Upadate();

		if (!player_->IsDead()) {
			player_->Update();
		}

		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
		}

		for (HitEffect* effect : hitEffects_) {
			effect->Update();
		}

		if (phase_ == Phase::kDeath && deathParticles_) {
			deathParticles_->Update();
			if (deathParticles_->IsFinished()) {
				phase_ = Phase::kFadeOut;
			}
		}

		if (phase_ == Phase::kFadeOut && fade_) {
			fade_->Update();
			if (fade_->IsFinished()) {
				finished_ = true;
			}
		}

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isDebugCameraActive = !isDebugCameraActive;
		}
#endif
		if (isDebugCameraActive) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		for (auto& line : worldTransformBlocks_) {
			for (WorldTransform*& block : line) {
				if (block) {
					WorldTransformUpdate(*block);
				}
			}
		}

		if (phase_ == Phase::kPlay) {
			CheckAllCollisions();
		}
		break;
	}

	// 死亡した敵の削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
}

// 当たり判定チェック
void GameScene::CheckAllCollisions() {
	AABB aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		if (enemy->IsCollisionDisabled())
			continue;
		AABB aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}

// ヒットエフェクト作成
void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	HitEffect* newEffect = HitEffect::Create(position);
	hitEffects_.push_back(newEffect);
}

// 描画
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	if (!player_->IsDead()) {
		player_->Draw();
	}

	skydome_->Draw();

	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (block)
				modelBlock->Draw(*block, camera_);
		}
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	for (HitEffect* effect : hitEffects_) {
		effect->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();

	if (fade_) {
		fade_->Draw();
	}
}
