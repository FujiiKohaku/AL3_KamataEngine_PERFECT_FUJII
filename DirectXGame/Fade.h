#pragma once
#include <KamataEngine.h>
#include <vector>

class Fade {
public:
	enum class Status {
		FadeIn,
		FadeOut,
	};

	void Initialize(float fadeSpeed = 1.0f);
	void Update();
	void Draw();

	void Start(Status status, float fadeSpeed);

	bool IsFinished() const { return isFinish_; }

private:
	enum State {
		kNone,
		kFadeIn,
		kFadeOut,
	};

	struct Tile {
		KamataEngine::Sprite* sprite = nullptr;
		float delay = 0.0f;
	};

	std::vector<Tile> tiles_;

	State state_ = kNone;

	int tileCountX_ = 0;
	int tileCountY_ = 0;
	int tileSize_ = 0;

	float fadeFrame_ = 60.0f;
	float fadeSpeed_ = 1.0f;
	int frame_ = 0;
	uint32_t textureHandle = 0;
	bool isFinish_ = false;
};
