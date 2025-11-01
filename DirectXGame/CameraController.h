#pragma once
#include <kamataEngine.h>
// 前方宣言
class Player;
class CameraController {
public:
	void Initialize(KamataEngine::Camera* camera);

	void Update();

	void Reset();
	//==========
	// セッター
	//==========
	void SetTarget(Player* target) { target_ = target; }

private:
	KamataEngine::Camera* camera_;

	Player* target_ = nullptr;

	// 矩形
	struct Rect {
		float left = 0.0f;
		float right = 0.0f;
		float bottom = 0.0f;
		float top = 0.0f;
	};

	// 追従対象とカメラの座標の差
	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};

	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_;

	// 座標補完割合 1frameでどのくらい目標座標に近づくか
	static inline const float kInterpolationRate = 0.1f;
	// 速度掛け率 1.0は等倍 プレイヤーのスピードが小さいほど、大きくする必要がある。
	static inline const float kVelocityBias = 15.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
