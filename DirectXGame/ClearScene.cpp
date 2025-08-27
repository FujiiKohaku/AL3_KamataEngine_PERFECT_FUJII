#include "ClearScene.h"

void ClearScene::Initialize() {
	camera_ = new KamataEngine::Camera();
	camera_->Initialize();

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void ClearScene::Update() {
	if (fade_)
		fade_->Update();

	// Enterキーでタイトルに戻る
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		goToTitle_ = true;
		finished_ = true;
	}
}

void ClearScene::Draw() {
	// 背景とか "CLEAR!!" モデルを描く
	if (fade_)
		fade_->Draw();
}
