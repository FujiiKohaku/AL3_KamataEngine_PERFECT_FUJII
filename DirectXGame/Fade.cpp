#include "Fade.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace KamataEngine;

void Fade::Initialize(float fadeSpeed) {

	fadeSpeed_ = fadeSpeed;
	fadeFrame_ = 60.0f;
	frame_ = 0;
	isFinish_ = false;
	state_ = kNone;

	srand((unsigned int)time(nullptr));

	// 1280x720 / 80
	tileCountX_ = 16;
	tileCountY_ = 9;
	tileSize_ = 80;

	textureHandle = TextureManager::Load("tile.png");

	tiles_.clear();

	for (int y = 0; y < tileCountY_; y++) {
		for (int x = 0; x < tileCountX_; x++) {

			Vector2 pos = {(float)(x * tileSize_), (float)(y * tileSize_)};

			Sprite* sprite = Sprite::Create(textureHandle, pos);
			sprite->SetSize({(float)tileSize_, (float)tileSize_});
			sprite->SetColor({0, 0, 0, 0.0f});

			Tile tile;
			tile.sprite = sprite;
			tile.delay = ((rand() % 100) / 100.0f) * fadeFrame_;

			tiles_.push_back(tile);
		}
	}
}

void Fade::Start(Status status, float fadeSpeed) {

	fadeSpeed_ = fadeSpeed;
	frame_ = 0;
	isFinish_ = false;

	if (status == Status::FadeIn) {
		state_ = kFadeIn;
		for (auto& tile : tiles_) {
			tile.delay = ((rand() % 100) / 100.0f) * fadeFrame_;
			tile.sprite->SetColor({0, 0, 0, 0.0f});
		}
	} else {
		state_ = kFadeOut;
		for (auto& tile : tiles_) {
			tile.delay = ((rand() % 100) / 100.0f) * fadeFrame_;
			tile.sprite->SetColor({0, 0, 0, 1.0f});
		}
	}
}

void Fade::Update() {

	if (state_ == kNone)
		return;

	frame_++;

	for (auto& tile : tiles_) {

		if (frame_ < tile.delay)
			continue;

		float t = (frame_ - tile.delay) / fadeFrame_;
		t = std::clamp(t, 0.0f, 1.0f);

		float alpha = 0.0f;

		if (state_ == kFadeIn) {
			alpha = t;
		} else {
			alpha = 1.0f - t;
		}

		tile.sprite->SetColor({0, 0, 0, alpha});
	}

	if (frame_ >= fadeFrame_ + 60) {

		if (state_ == kFadeIn) {
			for (auto& tile : tiles_) {
				tile.sprite->SetColor({0, 0, 0, 1.0f});
			}
		}

		isFinish_ = true;
	}
}

void Fade::Draw() {
	auto* dx = DirectXCommon::GetInstance();
	if (state_ == kNone)
		return;
	Sprite::PreDraw(dx->GetCommandList());
	for (auto& tile : tiles_) {
		tile.sprite->Draw();
	}
	Sprite::PostDraw();
}
