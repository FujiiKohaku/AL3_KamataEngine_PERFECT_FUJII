#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"
#include <Windows.h>

class Player;

class CameraController {
public:
	// ======= シングルトン =========
	static CameraController* GetInstance();
	static void Finalize();

	// 初期化
	void Initialize(KamataEngine::Camera* camera);

	// 毎フレーム更新
	void Update();

	// ターゲット設定
	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void SetMovableArea(const Rect& area) { movableArea_ = area; }
	void StartShake(float power = 0.5f, float time = 0.3f);
	// 位置取得
	const KamataEngine::Vector3& GetTranslate() const { return camera_->translation_; }
	// 位置設定
	void SetTranslate(const KamataEngine::Vector3& t) { camera_->translation_ = t; }

private:
	// ======= シングルトン =========
	static CameraController* instance_;
	CameraController() = default;
	~CameraController() = default;

private:
	KamataEngine::Camera* camera_ = nullptr;
	float shakePower_ = 0.0f;
	float shakeTimer_ = 0.0f;
	Player* target_ = nullptr;

	KamataEngine::Vector3 targetOffset_ = {0.0f, 3.0f, -15.0f};

	Rect movableArea_ = {0, 100, 0, 100};

	KamataEngine::Vector3 destination_;

	static inline const float kinterpolationRate = 0.1f;
	static inline const float kVelocityBias = 20.0f;
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
