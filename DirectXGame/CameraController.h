#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include "Math.h"
#include <Windows.h>
class Player; // 前方宣言
class CameraController {
public:
	// 初期化
	void Initialize(KamataEngine::Camera* camera);
	// 更新
	void Update();

	// ターゲットプレイヤーを設定
	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	struct Rect {
		float left = 0.0f;   // Xの最小
		float right = 1.0f;  // Xの最大
		float bottom = 0.0f; // Yの最小
		float top = 1.0f;    // Yの最大
	};

	void SetMovableArea(const Rect& area) { movableArea_ = area; }

private:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr; // プレイヤーへの参照

	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f}; // 追従対象とカメラの座標の差（オフセット）デフォルト-15.0f

	Rect movableArea_ = {0, 100, 0, 100}; // 一応デフォルト値入れてるけどgameScene.Cppで設定しているから意味なし
	// カメラ目標座標（座標補完）
	KamataEngine::Vector3 destination_; // 目的地

	static inline const float kinterpolationRate = 0.1f; // 補間率
};
