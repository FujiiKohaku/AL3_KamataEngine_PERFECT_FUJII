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

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};
};
