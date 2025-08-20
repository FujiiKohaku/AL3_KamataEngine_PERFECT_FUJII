#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"
class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void UpDate();

	void Draw();

	AABB GetAABB();

	Vector3 GetWorldPosition();

	void OnCollision(const Player* player);

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_;

	KamataEngine::Camera* camera_;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
