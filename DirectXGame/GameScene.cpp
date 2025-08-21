#include "GameScene.h"

using namespace KamataEngine;

//--------------------------------------------------
// デストラクタ
//--------------------------------------------------
GameScene::~GameScene() {
	delete player_;  // 先にプレイヤーを消す
	delete skydome_; // 先にスカイドームを消す

	delete model_;
	delete modelBlock_;
	delete skydomeModel_;

	delete debugCamera_;
	delete camera_;
	delete mapChipField_;

	delete cController_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear(); // 中のポインタも全部消して空っぽにする

	for (std::vector<WorldTransform*>& worldTransformBkockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBkockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

//==================================================
// 初期化
//==================================================
void GameScene::Initialize() {

	//--------------------------------------------------
	// モデル生成
	//--------------------------------------------------
	model_ = Model::CreateFromOBJ("playermax", true); // プレイヤーモデル
	modelRolling = Model::CreateFromOBJ("roll", true);
	modelBlock_ = Model::CreateFromOBJ("block", true);     // ブロックモデル
	skydomeModel_ = Model::CreateFromOBJ("skydome", true); // スカイドームモデル
	enemyModel_ = Model::CreateFromOBJ("enemy", true);     // エネミーモデル

	//--------------------------------------------------
	// ワールドトランスフォーム
	//--------------------------------------------------
	worldtransform_.Initialize();

	//--------------------------------------------------
	// カメラ関連
	//--------------------------------------------------
	camera_ = new Camera();
	camera_->Initialize();
	debugCamera_ = new DebugCamera(1080, 720);

	//--------------------------------------------------
	// テクスチャ読み込み
	//--------------------------------------------------
	textureHandle_ = TextureManager::Load("mario.jpg");

	//--------------------------------------------------
	// マップチップフィールド
	//--------------------------------------------------
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	//--------------------------------------------------
	// プレイヤー生成
	//--------------------------------------------------
	Vector3 playerposition = mapChipField_->GetMapChipPositionByIndex(5, 5);
	player_ = new Player();
	player_->Initialize(model_,modelRolling, camera_, playerposition);
	player_->SetMapChipField(mapChipField_);

	//--------------------------------------------------
	// ブロック生成
	//--------------------------------------------------
	GenerateBlocks();

	//--------------------------------------------------
	// スカイドーム生成
	//--------------------------------------------------
	skydome_ = new Skydome();
	skydome_->initialize(skydomeModel_, camera_);

	//--------------------------------------------------
	// カメラコントローラー
	//--------------------------------------------------
	cController_ = new CameraController();
	cController_->Initialize(camera_);
	cController_->SetTarget(player_);
	cController_->Reset();

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cController_->SetMovableArea(cameraArea);

	//--------------------------------------------------
	// 敵生成
	//--------------------------------------------------

	for (int32_t i = 0; i < 2; ++i) {
		Enemy* newEnemy = new Enemy();                                                    // 敵一体を作成
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14 + i * 2, 18); // マップの(14 + i*2, 18)の位置を調べて、その場所に敵を置く
		newEnemy->Initialize(enemyModel_, camera_, enemyPosition);                        // 敵を初期化（モデル・カメラ・位置をセット）
		enemies_.push_back(newEnemy);                                                     // 敵リスト(enemies_)に入れる
	}
}

//--------------------------------------------------
// 更新
//--------------------------------------------------
void GameScene::Update() {

	// デバッグカメラ切替（Debug時のみ）
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_; // ←トグルにする
	}
#endif

	// カメラ更新
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		camera_->TransferMatrix(); // ビュープロジェクション行列の転送
	} else {
		camera_->UpdateMatrix();
		cController_->Update(); // デバッグカメラのときは呼ばない
	}

	// プレイヤー更新
	player_->Update();
	// エネミー更新

	for (Enemy* enemy : enemies_) {
		enemy->UpDate();
	}
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;

			// アフィン変換～DirectXに転送
			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	skydome_->UpDate(); // スカイドーム更新
	CheckAllCollisions();
}

//--------------------------------------------------
// 描画
//--------------------------------------------------
void GameScene::Draw() {

	KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance();

	Model::PreDraw(dxcommon->GetCommandList());

	// モデル描画（プレイヤー自身で描画）
	// model_->Draw(worldtransform_, debugCamera_->GetCamera(), textureHandle_);
	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	// スカイドーム描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, *camera_, nullptr);
		}
	}
	Model::PostDraw();
}

#pragma region ブロック生成関数
// ブロック生成関数
void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
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
#pragma endregion

void GameScene::CheckAllCollisions() {

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	{
		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと敵弾全ての当たり判定
		for (Enemy* enemy : enemies_) {
			// 敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision(enemy);
				// 敵弾の衝突時コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}
	}
}
