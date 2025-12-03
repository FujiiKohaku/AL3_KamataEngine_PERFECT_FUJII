#pragma once
#include "KamataEngine.h"


using namespace KamataEngine;

class TitleScene {
public:
	void Initialize();
	void Update();
	void Draw();
	bool Finished() const { return finished_; }

private:
	bool finished_ = false;

	Model* model_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;
};
