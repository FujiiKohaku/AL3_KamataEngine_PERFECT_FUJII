#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"
/// <summary>
/// 壊れるブロック
/// </summary>
class BreakBlock {
public:
	// 初期化
	void Initialize(Model* model, const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw(Camera* camera);

	// プレイヤーとの当たり判定
	bool CheckCollision(const Player* player);

	// 攻撃・頭突きで壊す
	void OnHit();

	// 有効かどうか
	bool IsActive() const { return isActive_; }

private:
	// モデル
	Model* model_ = nullptr;

	// トランスフォーム
	WorldTransform worldTransform_;

	// アクティブ状態
	bool isActive_ = true;

	// 壊れたかどうか
	bool isBroken_ = false;

	// 壊れてからの経過時間
	float breakTimer_ = 0.0f;

	// 壊れるアニメーション継続時間
	const float kBreakDuration_ = 0.3f;

	// サイズ（当たり判定用）
	const float kBlockSize_ = 1.0f;
};
