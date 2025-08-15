#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
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

private: // C++ではメンバ変数は特別な理由がなければprivateにする
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
};