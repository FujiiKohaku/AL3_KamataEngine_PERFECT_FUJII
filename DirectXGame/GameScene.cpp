#include "GameScene.h"
#include "Math.h"
#include "TitleScene.h"

using namespace KamataEngine;

// ==============================
// コンストラクタ／デストラクタ
// ==============================

// コンストラクタ：メンバの初期値をまとめてセット
GameScene::GameScene()
    : model_(nullptr), modelAttack_(nullptr), player_(nullptr), mapChipField_(nullptr), debugCamera_(nullptr), skydome_(nullptr), cController_(nullptr), modelSkydome_(nullptr), modelBlock(nullptr),
      enemy_model_(nullptr), deathParticleModel(nullptr), deathParticles_(nullptr), modelPlayerHit_(nullptr), fade_(nullptr), isDebugCameraActive(false), finished_(false), phase_(Phase::kFadeIn) {}

// デストラクタ：所有オブジェクトを順に解放
GameScene::~GameScene() {
	delete model_;
	delete modelAttack_;
	delete player_;

	// ブロックWorldTransform（2次元vector内のnew）をすべてdelete
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

	// 敵の解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	// ヒットエフェクトの解放
	for (HitEffect* effect : hitEffects_) {
		delete effect;
	}
	hitEffects_.clear();

	delete deathParticles_;
	delete deathParticleModel;
	delete fade_;
}

// ==============================
// 初期化
// ==============================
void GameScene::Initialize() {
	// ---- マップとプレイヤー初期位置 ----
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// ---- モデル読み込み ----
	model_ = Model::CreateFromOBJ("player", true);
	modelAttack_ = Model::CreateFromOBJ("attack_effect", true);

	// ---- プレイヤー生成 ----
	player_ = new Player();
	player_->Initialize(model_, modelAttack_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// ---- カメラ関連 ----
	camera_.Initialize();

	cController_ = new CameraController();
	cController_->Initialize(&camera_);
	cController_->SetTarget(player_);
	cController_->Reset();

	// デバッグカメラ（CキーでON/OFF）
	debugCamera_ = new DebugCamera(1280, 720);

	// ---- スカイドーム ----
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// ---- ブロック生成 ----
	modelBlock = Model::CreateFromOBJ("block", true);
	GenerateBlocks();

	// ---- カメラの移動範囲 ----
	CameraController::Rect cameraArea = {12.0f, 88.0f, 6.0f, 6.0f};
	cController_->SetMoveableArea(cameraArea);

	// ---- 敵の初期化 ----
	enemy_model_ = Model::CreateFromOBJ("enemy", true);
	for (int i = 0; i < 2; ++i) {
		Enemy* enemy = new Enemy();
		Vector3 pos = mapChipField_->GetMapChipPositionByIndex(14 + i * 2, 18);
		enemy->Initialize(enemy_model_, &camera_, pos);
		enemy->SetGameScene(this); // コールバック用
		enemies_.push_back(enemy);
	}

	// ---- デスパーティクル ----
	deathParticleModel = Model::CreateFromOBJ("deathParticle", true);

	// ---- フェード ----
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f); // 最初はフェードイン

	// ---- ヒットエフェクト共有設定 ----
	modelPlayerHit_ = Model::CreateFromOBJ("particle", true);
	HitEffect::SetModel(modelPlayerHit_);
	HitEffect::SetCamera(&camera_);
}

// ==============================
// ブロック生成（マップ→WT2次元配列）
// ==============================
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

// ==============================
// フェーズ遷移（状態管理）
// ==============================
void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		// プレイ中→死亡検知でデス演出へ
		if (player_->IsDead()) {
			phase_ = Phase::kDeath;
			const Vector3& deathPos = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(deathParticleModel, &camera_, deathPos);
		}
		break;
	case Phase::kDeath:
		// デス演出中の遷移はUpdate側で処理
		break;
	default:
		break;
	}
}

// ==============================
// 更新（メインロジック）
// ==============================
void GameScene::Update() {
	// まずフェーズ遷移のトリガ確認
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		// フェードイン進行 → 終了後、Playへ（ここはfallthroughで続く）
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f); // 次のフェード（終了時に使う）
			phase_ = Phase::kPlay;
		}
		[[fallthrough]];

	case Phase::kPlay:
	case Phase::kDeath:
	case Phase::kFadeOut:
		// ---- 背景・カメラ ----
		skydome_->Update();
		cController_->Upadate(); // ※メソッド名の綴りは仕様どおり

		// プレイヤー（死亡中は更新しない）
		if (!player_->IsDead()) {
			player_->Update();
		}

		// 敵更新
		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
		}

		// ヒットエフェクト更新
		for (HitEffect* effect : hitEffects_) {
			effect->Update();
		}

		// デス演出 → 完了したらフェードアウトへ
		if (phase_ == Phase::kDeath && deathParticles_) {
			deathParticles_->Update();
			if (deathParticles_->IsFinished()) {
				phase_ = Phase::kFadeOut;
			}
		}

		// フェードアウト → 終了
		if (phase_ == Phase::kFadeOut && fade_) {
			fade_->Update();
			if (fade_->IsFinished()) {
				finished_ = true;
			}
		}

// ---- デバッグカメラ切替 ----
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isDebugCameraActive = !isDebugCameraActive;
		}
#endif

		if (isDebugCameraActive) {
			// デバッグカメラの行列をメインカメラへ反映
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		// ---- ブロックの行列更新 ----
		for (auto& line : worldTransformBlocks_) {
			for (WorldTransform*& block : line) {
				if (block) {
					WorldTransformUpdate(*block);
				}
			}
		}

		// ---- 当たり判定（プレイ中のみ）----
		if (phase_ == Phase::kPlay) {
			CheckAllCollisions();
		}
		break;
	}

	// ---- 死亡した敵の削除（remove_ifで同時にdelete）----
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
}

// ==============================
// 当たり判定
// ==============================
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

// ==============================
// ヒットエフェクト生成
// ==============================
void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	HitEffect* newEffect = HitEffect::Create(position);
	hitEffects_.push_back(newEffect);
}

// ==============================
// 描画
// ==============================
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ---- 3D描画パス ----
	Model::PreDraw(dxCommon->GetCommandList());

	if (!player_->IsDead()) {
		player_->Draw();
	}

	skydome_->Draw();

	// ブロック描画（存在するセルのみ）
	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (block) {
				modelBlock->Draw(*block, camera_);
			}
		}
	}

	// 敵／ヒットエフェクト／デス演出
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

	// ---- 2D描画パス（今回はスプライトなしでも前後を呼ぶ設計）----
	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();

	// ---- フェード（最前面）----
	if (fade_) {
		fade_->Draw();
	}
}
