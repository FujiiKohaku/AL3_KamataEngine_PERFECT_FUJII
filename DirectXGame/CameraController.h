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

	// 追従対象とカメラの座標の差
	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};

	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_;

	// 座標補完割合 1frameでどのくらい目標座標に近づくか
	static inline const float kInterpolationRate = 0.1f;
};
