#pragma once
#include "Math.h"
#include "KamataEngine.h"
#include <algorithm>
#include <numbers>


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
	// getter(02_06スライド11枚目で追加)
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

private: // C++ではメンバ変数は特別な理由がなければprivateにする
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 速度
	KamataEngine::Vector3 velocity_ = {};

	static inline float kAccelaration_ = 0.01f;

	static inline float kAttenuation = 0.05f;

	static inline float kLimitRunSpeed = 0.3f;

	// 左右
	enum class LRDorection {
		kRight,
		kLeft,
	};

	LRDorection lrDirection_ = LRDorection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.98f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 20.0f;
};
