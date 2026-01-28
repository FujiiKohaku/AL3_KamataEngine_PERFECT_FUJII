#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class Block {
public:
	void Initialize(Model* model, const Vector3& position, bool isBreakable);

	void Update();
	void Draw(Camera* camera);

	void Break();

	bool IsBroken() const;
	bool IsBreakable() const;

	const WorldTransform& GetWorldTransform() const;

	bool CheckHitSphere(const Vector3& pos, float radius) const;
	float GetRadius() const;

	void SetMapIndex(uint32_t x, uint32_t y);
	uint32_t GetMapX() const;
	uint32_t GetMapY() const;

	bool IsJustBroken() const;
	void ClearJustBroken();

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

	uint32_t mapX_ = 0;
	uint32_t mapY_ = 0;

	bool isBreakable_ = false;
	bool isBroken_ = false;
	bool justBroken_ = false;
};
