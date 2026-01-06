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

	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.UpdateMatrix();
	// Initialize
	basePos_ = worldTransformTitle_.translation_;

	modelGuru_ = Model::CreateFromOBJ("guruguru", true);
	worldTransformGuruGuru_.Initialize();
	worldTransformGuruGuru_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformGuruGuru_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformGuruGuru_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransformGuruGuru_.translation_.y = 2.0f;
	worldTransformGuruGuru_.translation_.z = -5.0f;

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
	worldTransformTitle_.translation_.y = basePos_.y + std::sinf(animTime_) * 0.3f;
	worldTransformGuruGuru_.rotation_.x += 0.02f;
	worldTransformGuruGuru_.rotation_.y += 0.03f;
	worldTransformGuruGuru_.rotation_.z += 0.01f;
	// 行列更新
	WorldTransformUpdate(worldTransformTitle_);
	WorldTransformUpdate(worldTransformGuruGuru_);
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
	modelGuru_->Draw(worldTransformGuruGuru_, camera_);
	skydome_->Draw();
	// 描画終了
	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	seSprite_->Draw();
	Sprite::PostDraw();
}
