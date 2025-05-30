#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <algorithm>
class Player;
// Al2_02_06
class CameraController {
public:
	// 矩形02_06
	struct Rect {
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};

	// Al2_02_06
	void Initialize(KamataEngine::Camera* camera);
	// Al2_02_06
	void Upadate();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMoveableArea(Rect area) { movableArea_ = area; }
	private:
	// カメラAl2_02_06
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差(オフセット)//Al3_02_06
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	// カメラ移動範囲02_06
	Rect movableArea_ = {0, 100, 0, 100};

	
    // 02_06スライド23枚目 目標座標
	Vector3 destination_;
	// 02_06スライド23枚目 座標補間割合
	static inline const float kInterpolationRate = 0.1f;

};
