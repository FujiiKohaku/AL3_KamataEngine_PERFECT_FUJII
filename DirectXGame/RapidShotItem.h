#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;
class Player;
class RapidShotItem {
public:
	void Initialize(Model* model, const Vector3& pos);
	void Update();
	void Draw(Camera* camera);

	bool IsCollected() const { return collected_; }
	void OnCollision(Player* player);

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool collected_ = false;
	float radius_ = 0.5f;

public:
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	float GetRadius() const { return radius_; }
};
