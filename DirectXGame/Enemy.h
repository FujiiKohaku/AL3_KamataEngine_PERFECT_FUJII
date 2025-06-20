#pragma once
#include "KamataEngine.h"
#include "Math.h"
class Enemy {
public:
	void UpDate();

	void Draw();

	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// モデルのセット関数（モデルを記録）
	void SetModel(Model* model) { model_ = model; }

private:
	// メンバ変数の追加
	// ワールドトランスフォーム（所有）
	WorldTransform worldTransform_;
	// モデルのポインタ(借りてくるよう)
	Model* model_ = nullptr;
	// カメラのポインタ(借りてくるよう)
	KamataEngine::Camera* camera_ = nullptr;
	// 02_09
	static inline const float kWalkSpeed = 0.02f;
	// 02_09
	Vector3 velocity_ = {};

	// 02_09 19枚目
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionTime = 1.0f;
	// 02_09 20枚目
	float walkTimer = 0.0f;
};
