#pragma once
#include "KamataEngine.h"
#include "Player.h"

/// <summary>
/// トゲ（当たると死亡）
/// </summary>
class Spike {
public:
	// 定数
	static constexpr float kSpikeRadius = 0.5f; // 当たり半径

public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(Camera* camera);

	// プレイヤーと当たり判定（円形）
	bool CheckCollision(const Player* player) const;

private:
	Model* model_ = nullptr;
	WorldTransform worldTransform_;
	bool isActive_ = true;
};
