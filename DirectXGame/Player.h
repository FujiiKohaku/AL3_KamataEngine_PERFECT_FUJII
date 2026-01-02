#pragma once
#include "KamataEngine.h"

#include "Bullet.h"
#include <vector>

using namespace KamataEngine;

class MapChipField;
class Coin;
class Spike;
class Goal;
class Enemy;
class Math;
class EnemyBase;
class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角 02_07スライド16枚目
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

	/// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// 更新
	void Update();

	/// 描画
	void Draw();

	// getter(02_06スライド11枚目で追加)
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 02_06スライド28枚目で追加
	const Vector3& GetVelocity() const { return velocity_; }

	// 02_07 スライド4枚目
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	bool IsDead() const { return deathState_ == DeathState::DeadFinish; }

	bool IsGoal() const { return isGoal_; }

	void OnCollision(Coin* coin);
	void OnCollision(Spike* spike);
	void OnCollision(Goal* goal);
	void OnCollision(EnemyBase* enemy);
	bool GetHitEnemy() const { return hitEnemy_; }

	float GetRadius() const { return radius_; }
	void StartInhale();

	void StopInhale();

private:
	struct HitBox {
		Vector3 pos;
		float radius = 1.0f; // 吸い込み範囲
		bool active = false;
	};

	HitBox inhaleHitBox_;
	enum class DeathState {
		Alive,      // 生きてる
		Dying,      // 演出中
		DeadFinish, // 完全終了
	};

	enum class PlayerState {
		Normal,  // ふつう
		Inhale,  // 吸い込み中
		Swallow, // 食べた後（将来用）
	};

	PlayerState state_ = PlayerState::Normal;
	DeathState deathState_ = DeathState::Alive;
	float deathTimer_ = 0.0f;
	Vector3 deathVelocity_ = {};

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	Camera* camera_ = nullptr;
	// 02_05 移動量
	Vector3 velocity_ = {};
	// 02_05  フレームごとの加速度
	static inline const float kAcceleration = 0.1f;
	// 02_05 非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;
	// 02_05 最高速度
	static inline const float kLimitRunSpeed = 0.3f;
	// 02_05 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 02_05 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 02_05 旋回タイマー
	float turnTimer_ = 0.0f;
	// 02_05 旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;
	// 02_05 着地フラグ
	bool onGround_ = true;
	// 02_05 ジャンプ定数もろもろ
	static inline const float kJumpAcceleration = 20.0f;
	static inline const float kGravityAcceleration = 0.98f;
	static inline const float kLimitFallSpeed = 0.5f;
	// ぽよぽよアニメーション用

	// 02_07 マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// 02_07 キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// 02_07スライド34枚目
	static inline const float kBlank = 0.04f;

	float radius_ = 0.4f;
	// 02_07スライド10枚目 移動入力
	void InputMove();
	// 02_07 スライド12枚目
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};
	// 02_07 スライド13枚目
	void CheckMapCollision(CollisionMapInfo& info);
	// 02_07 スライド14枚目
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	// 02_07 スライド17枚目
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 02_08スライド14枚目 設置状態の切り替え処理
	void UpdateOnGround(const CollisionMapInfo& info);

	// 02_08 スライド27枚目 壁接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);
	void AbsorbEnemy(EnemyBase* enemy);
	// 02_08スライド16枚目 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;
	// 02_08スライド21枚目 微小な数値
	static inline const float kGroundSearchHeight = 0.06f;
	// 02_08スライド27枚目 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;
	bool isGoal_ = false; // ゴールフラグ
	int jumpCount_ = 0;
	static inline const int kMaxJumpCount = 2;
	static inline const float kSecondJumpPowerScale = 0.75f;
	// 二段ジャンプ専用回転演出
	bool spinActive_ = false;    // 回転アニメ中？
	float spinTimer_ = 0.0f;     // 経過時間
	float spinDuration_ = 0.35f; // 回転完了までの時間(秒)
	bool inhaling_ = false;
	void StartDeath();
	float EaseOutCubic(float t);
	int hp_ = 3;
	bool invincible_ = false;
	bool visible_ = true;
	float invincibleTimer_ = 0.0f;
	void TakeDamage(const Vector3& enemyPos);

	bool hitEnemy_ = false;

public:
	const HitBox& GetInhaleHitBox() const { return inhaleHitBox_; }
};