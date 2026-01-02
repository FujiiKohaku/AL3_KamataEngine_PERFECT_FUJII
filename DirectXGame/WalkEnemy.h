#pragma once
#include "EnemyBase.h"

class WalkEnemy : public EnemyBase {
public:
	void Initialize(Model* model, const Vector3& pos) override;

protected:
	void UpdateNormal() override; 

private:
	Vector3 startPos_{};
	float direction_ = 1.0f;

	static inline const float kMoveSpeed = 0.05f;
	static inline const float kMoveRange = 3.0f;
};
