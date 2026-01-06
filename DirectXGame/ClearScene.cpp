#include "ClearScene.h"
#include "Math.h"
#include "imgui.h"

using namespace KamataEngine;

void ClearScene::Initialize() {
	// モデル読み込み（ファイル名はプロジェクトに合わせて変更してOK）
	model_ = Model::CreateFromOBJ("pushSpace", true);

	// ワールド行列初期化
	worldTransform_.Initialize();
	worldTransform_.rotation_.x = PI / 2.0f; // 前向きに調整
	worldTransform_.rotation_.y = PI;
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.UpdateMatrix();
	bgmHandle_ = Audio::GetInstance()->LoadWave("Clear.mp3");
	Audio::GetInstance()->PlayWave(bgmHandle_);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	clearSprite_ = Sprite::Create(TextureManager::Load("clear.png"), {400.0f, 100.0f});

}

void ClearScene::Update() {
	// スペースキーでシーン終了
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// モデルをゆっくり回転
	worldTransform_.rotation_.y += 0.01f;
	skydome_->Update();
	// 行列更新
	WorldTransformUpdate(worldTransform_);
	camera_.UpdateMatrix();

	
}

void ClearScene::Draw() {
	// コマンドリスト取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// 3D描画準備
	Model::PreDraw(commandList);

	// モデル描画
	if (model_) {
		model_->Draw(worldTransform_, camera_);
	}
	skydome_->Draw();
	// 3D描画終了
	Model::PostDraw();



	Sprite::PreDraw(commandList);
	clearSprite_->Draw();
	Sprite::PostDraw();
}
