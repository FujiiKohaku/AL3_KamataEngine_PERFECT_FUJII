#include "Fade.h"
#include <algorithm>

// ==============================
// 初期化
// ==============================
void Fade::Initialize() {
	// 黒い全画面スプライトを生成
	sprite_ = KamataEngine::Sprite::Create(0, KamataEngine::Vector2{});
	sprite_->SetSize(KamataEngine::Vector2(1280, 720));   // 画面サイズ
	sprite_->SetColor(KamataEngine::Vector4(0, 0, 0, 1)); // 黒・不透明
}

// ==============================
// 更新
// ==============================
void Fade::Update() {
	switch (status_) {
	case Fade::Status::None:
		// 何もしない
		break;

	case Fade::Status::FadeIn:
		// 経過時間を加算（1フレーム分）
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_; // 上限に固定
		}
		// 時間経過に応じてアルファ値を減少（透明になる）
		sprite_->SetColor(KamataEngine::Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));
		break;

	case Fade::Status::FadeOut:
		// 経過時間を加算（1フレーム分）
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 時間経過に応じてアルファ値を増加（黒くなる）
		sprite_->SetColor(KamataEngine::Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;

	default:
		break;
	}
}

// ==============================
// 描画
// ==============================
void Fade::Draw() {
	if (status_ == Status::None) {
		return; // フェード無効時は描画しない
	}

	KamataEngine::Sprite::PreDraw(KamataEngine::DirectXCommon::GetInstance()->GetCommandList());
	sprite_->Draw();
	KamataEngine::Sprite::PostDraw();
}

// ==============================
// 開始
// ==============================
void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

// ==============================
// 停止
// ==============================
void Fade::Stop() { status_ = Status::None; }

// ==============================
// 完了判定
// ==============================
bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		return (counter_ >= duration_);
	}
	return true; // Noneまたは不明状態は完了扱い
}
