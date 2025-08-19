#pragma once
#include "KamataEngine.h"
#include "Math.h"
class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void UpDate();

	void Draw();
	
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_;

	KamataEngine::Camera* camera_;
};
