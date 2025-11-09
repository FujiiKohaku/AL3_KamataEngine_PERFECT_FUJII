#pragma once
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;

class Enemy {
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(Camera* camera);

	// 当たり判定
	bool CheckCollision(Player* player) const;
	void OnCollision(Player* player); // 双方向対応

	// 死亡状態
	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Vector3 velocity_ = {};
	bool isDead_ = false;

	// 判定用半径
	static inline const float kEnemyRadius = 1.0f;
};
