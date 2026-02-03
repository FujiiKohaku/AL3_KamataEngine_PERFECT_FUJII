#pragma once
#include "Math.h"
#include <kamataEngine.h>
using namespace KamataEngine;

class BlockBreakEffect {
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(Camera* camera);
	bool IsAlive() const;

private:
	Model* blockBreakEffect_ = nullptr;
	WorldTransform worldTransform_;
	float timer_ = 0.0f;
	float lifeTime_ = 0.5f;
	Vector3 velocity_;
	Vector3 rotateVelocity_;
};
