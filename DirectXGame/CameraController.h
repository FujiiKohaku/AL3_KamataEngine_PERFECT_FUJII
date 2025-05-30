#pragma once
#include "KamataEngine.h"
#include "Math.h"

class Player;
// Al2_02_06
class CameraController {
public:
	// Al2_02_06
	void Initialize(KamataEngine::Camera* camera);
	// Al2_02_06
	void Upadate();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

private:
	// カメラAl2_02_06
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差(オフセット)//Al3_02_06
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
};
