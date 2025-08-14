#include "TitleScene.h"
#include "Math.h"
#include <cmath> // std::fmod, std::sin
#include <numbers>

TitleScene::~TitleScene() { delete fade_; }

void TitleScene::Initialize() {
	// モデル読み込み
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("player");

	// カメラ初期化
	camera_.Initialize();

	// タイトル文字のワールド設定
	constexpr float kPlayerTitle = 2.0f;
	worldTransformTitle_.Initialize();
	worldTransformTitle_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	// プレイヤーモデルのワールド設定
	constexpr float kPlayerScale = 10.0f;
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};
	worldTransformPlayer_.rotation_.y = 0.95f * std::numbers::pi_v<float>;
	worldTransformPlayer_.translation_ = {-2.0f, -10.0f, 0.0f};

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	// タイトルの上下アニメーション
	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeTitleMove);
	float angle = counter_ / kTimeTitleMove * 2.0f * std::numbers::pi_v<float>;
	worldTransformTitle_.translation_.y = std::sin(angle) + 10.0f;

	// カメラ更新
	camera_.TransferMatrix();

	// 行列転送
	WorldTransformUpdate(worldTransformTitle_);
	WorldTransformUpdate(worldTransformPlayer_);
}

void TitleScene::Draw() {
	auto* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	Model::PreDraw(commandList);
	modelTitle_->Draw(worldTransformTitle_, camera_);
	modelPlayer_->Draw(worldTransformPlayer_, camera_);
	fade_->Draw();
	Model::PostDraw();
}
