#pragma once
#include "Math.h"
#include "kamataEngine.h"

class Bullet {
public:
	
	void Initialize(Model* model, const Vector3& p, const Vector3& v);
	void Update();
	void Draw(Camera* camera);

	Vector3 GetPosition() const { return pos_; }
	void SetDead(bool d) { dead_ = d; }
	bool IsDead() const { return dead_; }

private:
	Model* model_ = nullptr;
	Vector3 pos_;
	Vector3 vel_;
	WorldTransform world_;
	bool dead_ = false;
	
};
