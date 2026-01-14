#pragma once
#include "KamataEngine.h"
#include "Skydome.h"
#include <string>
#include "StageState.h"
using namespace KamataEngine;

class SceneSelectScene {
public:
	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画
	void Draw();

	// 選択完了状態チェック
	bool Finished() const;

	// 選択されたステージCSVのパス
	const std::string& GetSelectedStagePath() const;

	int GetSelectedStageIndex() const { return selectedIndex_; }

	StageState GetSelectedStage() const { return stageState_; }

private:
	StageState stageState_ = StageState::Stage1;
	bool isFinished_ = false;
	std::string selectedStagePath_ = "";

	std::vector<std::string> stagePaths_;
	int selectedIndex_;

	Camera* camera_;
	float blockY_[3] = {0, 0, 0};
	float blockTargetY_[3] = {0, 0, 0};

	WorldTransform worldTransforms_[3] = {};
	Model* stageModel1_;
	Model* stageModel2_;
	Model* stageModel3_;

	Model* ModelPush_;
	WorldTransform worldTransformPush_;
	//---------------
	// skydome
	//---------------
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	Model* modelA_;
	WorldTransform worldTransformA_;

	Model* modelB_;
	WorldTransform worldTransformB_;

	uint32_t bgmHandle_ = 0;
	uint32_t playBgmHandle_ = 0;
};
