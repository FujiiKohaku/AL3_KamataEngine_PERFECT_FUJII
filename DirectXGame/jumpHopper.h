#pragma once
#include "KamataEngine.h"

class Player;
using namespace KamataEngine;

class JumpHopper {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& pos, float power);

	void Update(Player* player);
	void Draw();
	bool CheckCollision(Player* player);
	void Activate(Player* player); // 乗ったときの処理

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	bool wasInside_ = false;

	float power_ = 20.0f;   // どれだけ跳ねるか
	bool cooldown_ = false; // 連続発動を防ぐ
	float cooldownTimer_ = 0.0f;
	static inline const float kCooldownTime = 0.25f;
};
