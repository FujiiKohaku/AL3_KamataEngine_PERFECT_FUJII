#include "Fade.h"
#include <algorithm>

void Fade::Initialize() {
	// 02_13_page10
	// スプライト生成

	sprite_ = KamataEngine::Sprite::Create(0,KamataEngine::Vector2{});
	// サイズ指定
	sprite_->SetSize(KamataEngine::Vector2(1280,720));
	// 色指定
	sprite_->SetColor(KamataEngine::Vector4(0, 0, 0, 1));
}

void Fade::Update() {}

void Fade::Draw() {

	// よくわからんがなんかPSOセットしてシグネチャセットして設定をコマンドリストに反映
	KamataEngine::Sprite::PreDraw(KamataEngine::DirectXCommon::GetInstance()->GetCommandList());

	sprite_->Draw();

	KamataEngine::Sprite::PostDraw();
}
