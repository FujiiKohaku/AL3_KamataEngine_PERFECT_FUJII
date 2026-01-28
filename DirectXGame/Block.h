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

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

	bool isBreakable_ = false;
	bool isBroken_ = false;
};
