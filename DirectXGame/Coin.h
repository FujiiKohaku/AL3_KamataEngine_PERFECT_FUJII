#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"
using namespace KamataEngine;

class Coin {
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(Camera* camera);

	// 判定・状態
	bool CheckCollision(const Player* player) const;
	bool IsCollected() const { return collected_; }
	void SetCollected(bool collected) { collected_ = collected; }

	// 位置情報（外部アクセス用）
	const Vector3& GetPosition() const { return worldTransform_.translation_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool collected_ = false;     // 取得済みか
	float rotateSpeed_ = 180.0f; // 回転速度（度/秒）

public:
	static inline const float kCoinRadius = 0.9f;
};
