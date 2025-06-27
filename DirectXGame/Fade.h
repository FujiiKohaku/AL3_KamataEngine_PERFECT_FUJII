#pragma once
#include "KamataEngine.h"
class Fade {
public:
	void Initialize();

	void Update();

	void Draw();

private:
	// Spriteの生成タイミング制御のためポインタで保持
	KamataEngine::Sprite* sprite_ = nullptr;

	static const int kWidth = 1280;
	static const int kHeight = 720;
	
};
