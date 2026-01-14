#include "TitleScene.h"
#include "Math.h"
#include "imgui.h"

#include <numbers>
using namespace KamataEngine;

void TitleScene::Initialize() {
	// モデルを読み込む
	modelTitle_ = Model::CreateFromOBJ("pushSpace", true);
	worldTransformTitle_.Initialize();
	worldTransformTitle_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformTitle_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformTitle_.translation_.y = -4.0f;
	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.UpdateMatrix();
	// Initialize
	basePos_ = worldTransformTitle_.translation_;



	titleSprite_ = Sprite::Create(TextureManager::Load("title.png"), {-100.0f, -170.0f});
	worldTransformLogo_.Initialize();
	worldTransformLogo_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformLogo_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformLogo_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformLogo_.translation_.y = 2.0f;
	worldTransformLogo_.translation_.z = -5.0f;
	WorldTransformUpdate(worldTransformLogo_);



	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	bgmHandle_ = Audio::GetInstance()->LoadWave("title.mp3");
	bgmPlayHandle_=Audio::GetInstance()->PlayWave(bgmHandle_, true);
}

void TitleScene::Update() {
	// スペースキーでシーン終了
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
		Audio::GetInstance()->StopWave(bgmPlayHandle_);
	}

	// Update
	animTime_ += 0.05f;
	// 移動する対象　= 移動する対象の初期位置+ sin(対象を上下に)* 上下する幅

	// 行列更新
	WorldTransformUpdate(worldTransformTitle_);
	WorldTransformUpdate(worldTransformLogo_);
	camera_.UpdateMatrix();

	skydome_->Update();

	seSprite_ = Sprite::Create(TextureManager::Load("magou.png"), {0.0f, 0.0f});
}

void TitleScene::Draw() {
	// DirectXコマンドリスト取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// 描画準備
	Model::PreDraw(commandList);

	// モデル描画
	modelTitle_->Draw(worldTransformTitle_, camera_);
	skydome_->Draw();
	// 描画終了
	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());
	titleSprite_->Draw();
	seSprite_->Draw();
	Sprite::PostDraw();
}
