#include "GameOverScene.h"
#include "Math.h"

using namespace KamataEngine;

void GameOverScene::Initialize() {
	// モデルを読み込む（ファイル名は実際のリソースに合わせて変更）
	model_ = Model::CreateFromOBJ("pushSpace", true);

	// ワールド行列初期化
	worldTransform_.Initialize();
	worldTransform_.rotation_.x = PI / 2.0f; // 前向きに
	worldTransform_.rotation_.y = PI;
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.UpdateMatrix();
}

void GameOverScene::Update() {
	// スペースキーでシーン終了
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// モデルをゆっくり回転
	worldTransform_.rotation_.y += 0.01f;

	// 行列更新
	WorldTransformUpdate(worldTransform_);
	camera_.UpdateMatrix();
}

void GameOverScene::Draw() {
	// コマンドリスト取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// 3D描画準備
	Model::PreDraw(commandList);

	// モデル描画
	if (model_) {
		model_->Draw(worldTransform_, camera_);
	}

	// 3D描画終了
	Model::PostDraw();
}
