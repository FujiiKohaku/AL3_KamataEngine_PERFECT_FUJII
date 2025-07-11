#include "GameScene.h"
#include "Math.h"
#include "TitleScene.h"
using namespace KamataEngine;

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
	// マップチップフィールドデリーと
	delete mapChipField_;

	// 02_10 6枚目 敵クラス削除
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	delete deathParticles_;

	delete deathParticleModel;
}

// 初期化
void GameScene::Initialize() {

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	// 3Dモデルデータの生成
	model_ = Model::Create();
	model_ = Model::CreateFromOBJ("player", true);
	//  攻撃モデルデータの生成
	modelAttack_ = Model::Create();
	modelAttack_ = Model::CreateFromOBJ("attack_effect", true);

	// カメラの初期化
	camera_.Initialize();
	// マップチップをnewするマップチップフィールドの初期化
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	// 自キャラの生成
	player_ = new Player();
	player_->Initialize(model_, modelAttack_,&camera_, playerPosition);
	modelBlock = Model::CreateFromOBJ("block", true);
	player_->SetMapChipField(mapChipField_);

	cController_ = new CameraController();
	cController_->Initialize(&camera_);
	cController_->SetTarget(player_);
	cController_->Reset();

	debugCamera_ = new DebugCamera(1280, 720);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cController_->SetMoveableArea(cameraArea);

	enemy_model_ = Model::CreateFromOBJ("enemy");

	for (int32_t i = 0; i < 2; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14 + i * 2, 18);
		newEnemy->Initialize(enemy_model_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	deathParticleModel = Model::CreateFromOBJ("deathParticle");

	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

//void GameScene::ChangePhase() {
//	switch (phase_) {
//	case Phase::kPlay:
//		if (player_->IsDead()) {
//			phase_ = Phase::kDeath;
//			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
//		}
//		break;
//	case Phase::kDeath:
//
//			break;
//	}
//}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetBlockHeight();
	uint32_t numBlockHorizontal = mapChipField_->GetBlockWidth();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
// 02_12 10枚目 GameScene::Update関数で呼び出しておく
// player->draw();をif(!player_->IsDead()){}で囲む
void GameScene::ChangePhase() {

	switch (phase_) {
	case Phase::kPlay:
		// 02_12 13枚目 if文から中身まで全部実装
		// Initialize関数のいきなりパーティクル発生処理は消す
		if (player_->IsDead()) {
			// 死亡演出
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(deathParticleModel, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
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
		skydome_->Update();
		cController_->Upadate();
		player_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
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

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				WorldTransformUpdate(*worldTransformBlock);
			}
		}
		break;
	case Phase::kPlay:
		skydome_->Update();
		cController_->Upadate();
		player_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
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

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		CheckAllCollisions();
		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}

		skydome_->Update();
		cController_->Upadate();

		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
		}

		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		skydome_->Update();
		cController_->Upadate();

		for (Enemy* enemy : enemies_) {
			enemy->UpDate();
		}
		break;
	}



}

// 当たり判定
void GameScene::CheckAllCollisions() {
	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	{
		aabb1 = player_->GetAABB();

		for (Enemy* enemy : enemies_) {
			aabb2 = enemy->GetAABB();

			if (IsCollision(aabb1, aabb2)) {
				player_->OnCollision(enemy);
				enemy->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

// フェーズ切り替え関数（重複）
//void GameScene::ChangePhese() {
//	switch (phase_) {
//	case GameScene::Phase::kPlay:
//		if (player_->IsDead()) {
//			phase_ = Phase::kDeath;
//			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
//			deathParticles_ = new DeathParticles;
//			deathParticles_->Initialize(deathParticleModel, &camera_, deathParticlesPosition);
//		}
//		break;
//	case GameScene::Phase::kDeath:
//		break;
//	default:
//		break;
//	}
//}
void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画
	if (!player_->IsDead())
		player_->Draw();

	// 天球描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock->Draw(*worldTransformBlock, camera_);
		}
	}

	// 02_09 12枚目 敵更新 → 02_10 7枚目で更新
	//	enemy_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// 02_11 18枚目 デスパーティクルあれば描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();

	// 02_13 28枚目
	fade_->Draw();
}
// コンストラクタ
GameScene::GameScene() {}
