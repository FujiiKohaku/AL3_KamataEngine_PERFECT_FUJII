#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
class MapChipField;

float Lerp(float x1, float x2, float t);

float EaseInOut(float x1, float x2, float t);
class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// コンストラクタ
	Player();
	// デストラクタ
	~Player();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void InputMove();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; };

private:
	// C++ではメンバ変数は特別な理由がなければprivateにする
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.05f; // 加速度

	static inline const float kAtteenuation = 0.3f; // 減衰率

	static inline const float kMaxSpeed = 0.5f; // 最大速度
	// 左右判別
	enum class LRDirection { kLeft, kRight, kNone };

	LRDirection lrDirection_ = lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;

	// ジャンプについて
	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.98f / 60.0f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.3f;
	// ジャンプ初速(上方向)
	static inline const float kjumpAcceleration = 20.0f / 60.0f;

	// キャラクターの当たり判定サイズ
	// 一マスを通りやすくするために0.8にする
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr; // マップチップフィールド

	struct CollisionMapInfo {
		bool isHitCeiling = false; // 天井に当たったか
		bool isHitFloor = false;   // 床に当たったか
		bool isHitWall = false;    // 壁に当たったか
		Vector3 move;
	};

	// 角
	enum Corner {
		kLeftTop,     // 左上
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kRightBottom, // 右下
		kNumCorners   // 角の数
	};
	// map衝突判定
	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);

	void CheckMapCollisionDown(CollisionMapInfo& info);

	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void CheckMapCollisionRight(CollisionMapInfo& info);

	// 角の位置を取得
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	// 隙間ふふふ
	static inline const float kBlank = 0.04f;
};