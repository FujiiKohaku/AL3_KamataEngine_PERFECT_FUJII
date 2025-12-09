#pragma once
#include "KamataEngine.h"
#include <string>
using namespace KamataEngine;

class SceneSelectScene {
public:
	// 初期化
	void Initialize();

	// 更新処理（入力で選択）
	void Update();

	// 描画（お好みで）
	void Draw();

	// 選択完了状態チェック
	bool Finished() const;

	// 選択されたステージCSVのパス
	const std::string& GetSelectedStagePath() const;

private:
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
};
