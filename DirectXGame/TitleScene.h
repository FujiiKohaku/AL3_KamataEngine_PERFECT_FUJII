#pragma once
#include "KamataEngine.h"
#include "Skydome.h"
using namespace KamataEngine;

class TitleScene {
public:
	void Initialize();
	void Update();
	void Draw();
	bool Finished() const { return finished_; }

private:
	bool finished_ = false;
	float animTime_ = 0.0f;
	Vector3 basePos_;
	Model* modelTitle_ = nullptr;
	WorldTransform worldTransformTitle_;
	Camera camera_;
	//タイトルロゴ
	Model* modelGuru_ = nullptr;
	WorldTransform worldTransformGuruGuru_;
	//---------------
	// skydome
	//---------------
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;


	// Audio
	uint32_t bgmHandle_ = 0;
	uint32_t bgmPlayHandle_ = 0;

	Sprite* seSprite_ = nullptr;
};
