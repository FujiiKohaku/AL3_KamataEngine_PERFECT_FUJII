#pragma once
#include "EnemyBase.h"

class WalkEnemy : public EnemyBase {
public:
	void Initialize(Model* model, const Vector3& pos) override;
	void OnHit() override; // 攻撃が当たった時に呼ぶ
protected:
	void UpdateNormal() override;

private:
	Vector3 startPos_{};
	float direction_ = 1.0f;

	static inline const float kMoveSpeed = 0.05f;
	static inline const float kMoveRange = 3.0f;
	int hp_ = 2; // ★ 2回で死ぬ
	bool isStaggering_ = false;
	float staggerTimer_ = 0.0f;

	static constexpr float kStaggerTime = 0.3f;
	Vector3 staggerVelocity_;
	float staggerShakeTimer_ = 0.0f;
};
