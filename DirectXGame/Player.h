#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <numbers>
class MapChipField;
class Player {

private:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool isHitCeiling = false;
		bool isOnGround = false;
		bool isHitWall = false;
		KamataEngine::Vector3 move;
	};
	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

public:
	Player();
	~Player();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 position);
	void Update();
	void Draw();

	void UpdateMove();
	// マップ衝突判定
	void CheckMapCollision(CollisionMapInfo& info);
	// マップ衝突判定上下左右
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	//指定したオブジェクトの角を取得する関数
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
	//=============
	// getter
	//=============
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	//=============
	// setter
	//=============
	void SetMapChipField(MapChipField* mapChipField) { mapChipField = mapChipField_; }

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0, 0, 0};

	// 定数
	const float kMoveSpeed = 0.3f; // 移動速度
	const float kJumpPower = 0.4f; // ジャンプ力
	const float kGravity = 0.02f;  // 重力加速度
	const float kGroundY = 0.0f;   // 地面の高さ
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	bool onGround_ = true;    // 接地フラグ
	bool facingRight_ = true; // 向き（右向き:true / 左向き:false）
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
};
