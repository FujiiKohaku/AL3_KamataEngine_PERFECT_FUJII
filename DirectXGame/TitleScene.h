#pragma once
#include "Fade.h"
#include "KamataEngine.h"
using namespace KamataEngine;

// 02_12 19枚目 タイトルシーン
class TitleScene {
public:
	~TitleScene();

	void Initialize();

	void Update();

	void Draw();

	// 02_12 26枚目
	bool IsFinished() const { return finished_; }

	// 02_12 27枚目 シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

private:
	static inline const float kTimeTitleMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformPlayer_;
	WorldTransform worldTransformSun_;
	WorldTransform worldTransformBack_;
	WorldTransform worldTransformPushSpace_;
	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;
	Model* sun_ = nullptr;
	Model* backGround_;
	Model* pushSpace_;
	uint32_t BackGroundTexture_;
	float counter_ = 0.0f;
	// 02_12 26枚目
	bool finished_ = false;
	// float counter_ = 0.0f;
	Fade* fade_ = nullptr;
	// 02_13 27枚目 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
	float angle_ = 0.0f;
};