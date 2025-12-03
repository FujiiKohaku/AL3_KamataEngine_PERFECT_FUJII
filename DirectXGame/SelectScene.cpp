#include "SelectScene.h"

void SceneSelectScene::Initialize() {
	selectedStagePath_.clear();
	isFinished_ = false;
}

void SceneSelectScene::Update() {

	// ステージ1
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_1)) {
		selectedStagePath_ = "Resources/map/stage1.csv";
		isFinished_ = true;
	}

	// ステージ2
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_2)) {
		selectedStagePath_ = "Resources/map/stage2.csv";
		isFinished_ = true;
	}

	// ステージ3
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_3)) {
		selectedStagePath_ = "Resources/map/stage3.csv";
		isFinished_ = true;
	}
}

void SceneSelectScene::Draw() {
	// 必要ならボタンや文字描画をここに書く
	// 例：ImGui で「1：ステージ1」みたいに表示
}

bool SceneSelectScene::Finished() const { return isFinished_; }

const std::string& SceneSelectScene::GetSelectedStagePath() const { return selectedStagePath_; }
