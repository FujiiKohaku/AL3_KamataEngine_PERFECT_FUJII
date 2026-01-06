#pragma once
#include "EnemyBase.h"

class ChaserEnemy : public EnemyBase {
public:
	void Initialize(Model* model, const Vector3& pos, Player* player);

protected:
	void UpdateNormal() override; // 待機 or 追尾処理

private:
	Player* player_ = nullptr;

	float detectRange_ = 20.0f;                          // 見つける距離
	float chaseSpeed_ = 0.06f; // 追尾速度
	bool chasing_ = false;     // 追尾中かどうか
};
