#pragma once
#include "KamataEngine.h"
#include "Player.h"
using namespace KamataEngine;

class Enemy {
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(Camera* camera);

	bool CheckCollision(Player* player) const;
	void OnCollision(Player* player);
	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool isDead_ = false;

	// 👇追加ここから
	Vector3 startPos_;                             // 初期位置
	float direction_ = 1.0f;                       // 移動方向（1:右, -1:左）
	static inline const float kMoveSpeed = 0.05f;  // 移動スピード
	static inline const float kMoveRange = 3.0f;   // 往復距離
	static inline const float kEnemyRadius = 1.0f; // 当たり判定半径
	float velocity_ = 0.0f;                       // 垂直方向の速度
};
