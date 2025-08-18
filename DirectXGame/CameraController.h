#pragma once
#include "KamataEngine.h"

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

private:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr; // プレイヤーへの参照

	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f}; // 追従対象とカメラの座標の差（オフセット）
};
