#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "Random.h"
class HitEffect {
public:
	// 初期化
	void Initialize(const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	// 静的メンバ関数
	// もデリ
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	// カメラ
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	// インスタンス生成と初期化
	static HitEffect* Create(const KamataEngine::Vector3& position);

	KamataEngine::Vector3 postion;
	

private:
	// モデル(借りてくる用)
	static KamataEngine::Model* model_;
	// カメラ(借りてくる用)
	static KamataEngine::Camera* camera_;
	// 円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

	std::array<KamataEngine::WorldTransform, 2> elllipseWorldTransforms_;

};
