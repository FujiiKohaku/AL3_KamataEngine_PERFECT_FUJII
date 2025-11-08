#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"
using namespace KamataEngine;

class Goal {
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(Camera* camera);

	// 判定に使う情報だけ公開
	const Vector3& GetPosition() const { return worldTransform_.translation_; }

	bool IsReached() const { return goalReached_; }
	void SetReached(bool reached) { goalReached_ = reached; }
	bool CheckCollision(const Player* player) const;

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool goalReached_ = false;
	float rotateSpeed_ = 1.0f;

public:
	static inline const float kGoalRadius = 1.0f;
};
