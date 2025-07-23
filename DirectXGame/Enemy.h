#pragma once
#include "KamataEngine.h"
#include "Math.h"
// 02_10_20枚
class Player;
class GameScene;
class Enemy {
public:
	void UpDate();

	void Draw();
	// 02_10 14枚目
	AABB GetAABB();

	// 02_10 スライド20枚目 衝突応答
	void OnCollision(const Player* player);

	// Enemy.h
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// モデルのセット関数（モデルを記録）
	void SetModel(Model* model) { model_ = model; }
	// 02_10 スライド14枚目 ワールド座標を取得
	Vector3 GetWorldPosition();

	bool IsDead() const { return isDead_; } // これゲッター

	enum class Behavior {
		kUnnown,  // 無効
		kWalk,    // 歩行
		kDefeated // 敗北

	};
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }
	// gameSceneのセッター
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	// メンバ変数の追加
	// ワールドトランスフォーム（所有）
	WorldTransform worldTransform_;
	// モデルのポインタ(借りてくるよう)
	Model* model_ = nullptr;
	// カメラのポインタ(借りてくるよう)
	KamataEngine::Camera* camera_ = nullptr;
	// 02_09
	static inline const float kWalkSpeed = 0.02f;
	// 02_09
	Vector3 velocity_ = {};

	// 02_09 19枚目
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionTime = 1.0f;
	// 02_09 20枚目
	float walkTimer = 0.0f;
	// 02_10 14枚目 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// 死亡フラグ_02_15_6枚目

	bool isDead_ = false;

	// 02_15_13枚目
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnnown;
	float counter_ = 0.0f; // カウンター
	                       // 02_15 15枚目
	static inline const float kDefeatedTime = 0.6f;
	static inline const float kDefeatedMotionAngleStart = 0.0f;
	static inline const float kDefeatedMotionAngleEnd = -60.0f;

	bool isCollisionDisabled_ = false; // 02_15

	GameScene* gameScene_ = nullptr;

	std::array<KamataEngine::WorldTransform, 2> elllipseWorldTransforms_;
};
