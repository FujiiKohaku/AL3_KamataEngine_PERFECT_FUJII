#pragma once
#include "KamataEngine.h"
#include "Player.h"
using namespace KamataEngine;

class EnemyBase {
public:
	virtual ~EnemyBase() = default;

	virtual void Initialize(Model* model, const Vector3& pos);
	void Update();
	void Draw(Camera* camera);
	void OnCollision(Player* player);
	void StartPulled(Player* player);

	bool IsDead() const { return isDead_; }
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	float GetRadius() const { return kEnemyRadius; }

protected:
	// 敵ごとに違う動きだけココで作る
	virtual void UpdateNormal() {}

	void UpdatePulled();

protected:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Player* target_ = nullptr;

	enum class State { Normal, Pulled, Dead };
	State state_ = State::Normal;

	bool isDead_ = false;
	float kEnemyRadius= 1.0f;
};
