#pragma once
#define NOMINMAX
#include "Fade.h"
#include "KamataEngine.h"
#include "Math.h"
class ClearScene {
public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }
	bool GoToTitle() const { return goToTitle_; }

private:
	KamataEngine::Camera* camera_ = nullptr;
	Fade* fade_ = nullptr;
	bool finished_ = false;
	bool goToTitle_ = false;
};
