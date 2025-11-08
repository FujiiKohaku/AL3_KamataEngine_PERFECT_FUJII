#pragma once
#include "KamataEngine.h"

class Fade {
public:
	void Initialize();
	void Update();
	void Draw();

	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	void Start(Status status, float duration);
	void Stop();
	bool IsFinished() const;


	Status GetStatus() const { return status_; }

	struct Tile {
		KamataEngine::Sprite* sprite;
		float delay;
	};

private:
	static const int kWidth = 1280;
	static const int kHeight = 720;

	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;

	const int screenWidth = 1280;
	const int screenHeight = 720;
	const int tileSize = 80;

	int tileCountX = screenWidth / tileSize + 1;
	int tileCountY = screenHeight / tileSize + 1;

	std::vector<Tile*> tiles_;
};
