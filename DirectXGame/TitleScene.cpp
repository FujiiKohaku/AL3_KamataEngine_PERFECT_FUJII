#include "TitleScene.h"
#include "Math.h"
#include "imgui.h"

using namespace KamataEngine;

void TitleScene::Initialize() {
	// モデルを読み込む
	model_ = Model::CreateFromOBJ("pushSpace", true);
	worldTransform_.Initialize();
	worldTransform_.rotation_.x = PI / 2.0f; // X軸90度回転
	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.UpdateMatrix();
}

void TitleScene::Update() {
	// スペースキーでシーン終了
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// 回転アニメーション
	worldTransform_.rotation_.y += 0.01f;

	// 行列更新
	WorldTransformUpdate(worldTransform_);
	camera_.UpdateMatrix();


}

void TitleScene::Draw() {
	// DirectXコマンドリスト取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// 描画準備
	Model::PreDraw(commandList);

	// モデル描画
	model_->Draw(worldTransform_, camera_);

	// 描画終了
	Model::PostDraw();
}
