#pragma once
#include "EnemyBase.h"

class EnemyFlyer : public EnemyBase {
public:
	void Initialize(Model* model, const Vector3& pos) override;

protected:
	void UpdateNormal() override; // 通常時の動き

private:
	Vector3 center_; // 飛び回る中心
	float timer_ = 0.0f;
	float radius_ = 2.0f; // 横方向の移動範囲
	float height_ = 1.0f; // 上下の揺れ幅
};
