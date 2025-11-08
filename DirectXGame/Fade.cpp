#include "Fade.h"
#include <algorithm>
void Fade::Initialize() {
	textureHandle_ = KamataEngine::TextureManager::Load("stile.png");

	for (int x = 0; x < tileCountX; x++) {
		for (int y = 0; y < tileCountY; y++) {
			KamataEngine::Sprite* sprite = KamataEngine::Sprite::Create(textureHandle_, {(float)(x * tileSize), (float)(y * tileSize)});
			sprite->SetSize({(float)tileSize, (float)tileSize});

			Tile* tile = new Tile();
			tile->sprite = sprite;
			tile->delay = ((rand() % 100) / 100.0f) * duration_;

			tiles_.push_back(tile);
		}
	}
}

void Fade::Update() {
	switch (status_) {
	case Fade::Status::None:
		// 何もしない
		break;

	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		// フェード時間の進行
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		for (auto& tile : tiles_) {
			// --- 遅延チェック ---
			if (counter_ < tile->delay) {
				// まだ始まらない
				if (status_ == Fade::Status::FadeIn) {
					tile->sprite->SetColor(KamataEngine::Vector4(1, 1, 1, 1.0f)); // 白のまま
				} else {
					tile->sprite->SetColor(KamataEngine::Vector4(1, 1, 1, 0.0f)); // 透明のまま
				}
				continue; // 次のタイルへ
			}

			// --- タイル開始後の経過時間 ---
			float t = counter_ - tile->delay;

			// このタイルがフェードできる残り時間（duration - delay）
			float effectiveDuration = duration_ - tile->delay;
			if (effectiveDuration <= 0) {
				effectiveDuration = 0.0001f; // 0除算防止
			}

			float alpha = 1.0f;

			if (status_ == Fade::Status::FadeIn) {
				// フェードイン: 白(1) → 透明(0)
				if (t < effectiveDuration) {
					alpha = 1.0f - (t / effectiveDuration);
				} else {
					alpha = 0.0f; // 完全透明
				}
			} else if (status_ == Fade::Status::FadeOut) {
				// フェードアウト: 透明(0) → 白(1)
				if (t < effectiveDuration) {
					alpha = (t / effectiveDuration);
				} else {
					alpha = 1.0f; // 完全白
				}
			}

			// 計算したアルファをスプライトに反映
			tile->sprite->SetColor(KamataEngine::Vector4(1, 1, 1, alpha));
		}
		break;
	}
}

void Fade::Draw() {

	// 02_13 24枚目
	if (status_ == Status::None) {
		return;
	}
	// よくわからんがなんかPSOセットしてシグネチャセットして設定をコマンドリストに反映
	KamataEngine::Sprite::PreDraw(KamataEngine::DirectXCommon::GetInstance()->GetCommandList());

	for (auto& tile : tiles_) {
		tile->sprite->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}
void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;

	// duration_ が決まったタイミングで delay を再計算
	for (auto& tile : tiles_) {
		tile->delay = ((rand() % 100) / 100.0f) * duration_;
	}
}
// フェード停止関数
void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {

	switch (status_) {
	case Status::FadeIn:
	case Fade::Status::FadeOut:

		if (counter_ >= duration_) {
			return true;
		} else {

			return false;
		}
	}

	return true;
}