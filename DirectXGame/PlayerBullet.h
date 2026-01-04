#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class PlayerBullet {
public:
	void Initialize(Model* model, const Vector3& pos, const Vector3& dir);
	void Update();
	void Draw(Camera* camera);

	bool IsAlive() const { return alive_; }

	float GetRadius() const { return radius_; }
	const WorldTransform& GetWorld() const { return world_; }

	void Kill() { alive_ = false; } // ← 名前を直す！

private:
	Model* model_ = nullptr;
	WorldTransform world_{};
	Vector3 direction_{};
	float speed_ = 0.0f;
	bool alive_ = false;
	float radius_ = 0.4f;
};
