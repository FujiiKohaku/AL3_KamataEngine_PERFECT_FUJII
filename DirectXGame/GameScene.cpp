#include "GameScene.h"
#include "Math.h"
#include "imgui.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() {
	// プレイヤーの初期座標
	Vector3 playerPosition = {1.0f, 1.0f, 1.0f};
	Vector3 BulletPosition = {1.0f, 1.0f, 1.0f};
	// 3Dモデル
	model_ = Model::Create();
	model_ = Model::CreateFromOBJ("player", true);

	// カメラ
	camera_.Initialize();

	// プレイヤー生成
	player_ = new Player();
	player_->Initialize(model_, &camera_, playerPosition);

	// ブロックモデル
	modelBlock = Model::CreateFromOBJ("block", true);

	// カメラコントローラ
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

	// カメラ移動範囲
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cController_->SetMoveableArea(cameraArea);

	modelBullet_ = Model::CreateFromOBJ("block");


}

// 更新
void GameScene::Update() {
	player_->Update();

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
		cController_->Update();
	}

	skydome_->Update();


}

// 描画
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3D描画開始
	model_->PreDraw(dxCommon->GetCommandList());

	// 3Dオブジェクト
	player_->Draw();
	skydome_->Draw();

	// 3D描画終了
	model_->PostDraw();
}

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete debugCamera_;
	delete skydome_;

}
