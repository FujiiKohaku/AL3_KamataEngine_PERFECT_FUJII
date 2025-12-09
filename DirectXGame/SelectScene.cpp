#include "SelectScene.h"
#include "Math.h"
using namespace KamataEngine;

void SceneSelectScene::Initialize() {
	selectedStagePath_.clear();
	isFinished_ = false;
	selectedIndex_ = 0;
	isFinished_ = false;

	stagePaths_ = {"Resources/map/stage1.csv", "Resources/map/stage2.csv", "Resources/map/stage3.csv"};
	camera_ = new Camera();
	camera_->Initialize();
	camera_->UpdateMatrix();
	//camera_->translation_.z = -60.0f;
	worldTransforms_[0].Initialize();
	worldTransforms_[0].translation_ = {-3, 0, -20};

	worldTransforms_[1].Initialize();
	worldTransforms_[1].translation_ = {0, 0, -20};

	worldTransforms_[2].Initialize();
	worldTransforms_[2].translation_ = {3, 0, -20};

	stageModel1_ = KamataEngine::Model::CreateFromOBJ("block");
	stageModel2_ = KamataEngine::Model::CreateFromOBJ("block");
	stageModel3_ = KamataEngine::Model::CreateFromOBJ("block");
	camera_->TransferMatrix();
}

void SceneSelectScene::Update() {


	
	// ← 上キー
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_LEFT)) {
		selectedIndex_--;
		if (selectedIndex_ < 0)
			selectedIndex_ = (int)stagePaths_.size() - 1;
	}

	// → 下キー
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
		selectedIndex_++;
		if (selectedIndex_ >= (int)stagePaths_.size())
			selectedIndex_ = 0;
	}

	// SPACEで決定
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		selectedStagePath_ = stagePaths_[selectedIndex_];
		isFinished_ = true;
	}
	for (int i = 0; i < 3; i++) {
		// target
		if (i == selectedIndex_) {
			blockTargetY_[i] = 1.0f; // 選択中は上へ
		} else {
			blockTargetY_[i] = 0.0f; // 非選択は下へ
		}

		// easing (めっちゃ簡単式)
		blockY_[i] += (blockTargetY_[i] - blockY_[i]) * 0.1f;
	}
	camera_->TransferMatrix();
	WorldTransformUpdate(worldTransforms_[0]);
	WorldTransformUpdate(worldTransforms_[1]);
	WorldTransformUpdate(worldTransforms_[2]);
}

void SceneSelectScene::Draw() {
	// ステージ1描画
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	worldTransforms_[0].translation_.y = blockY_[0];
	stageModel1_->Draw(worldTransforms_[0], *camera_);

	worldTransforms_[1].translation_.y = blockY_[1];
	stageModel2_->Draw(worldTransforms_[1], *camera_);

	worldTransforms_[2].translation_.y = blockY_[2];
	stageModel3_->Draw(worldTransforms_[2], *camera_);
	Model::PostDraw();
}

bool SceneSelectScene::Finished() const { return isFinished_; }

const std::string& SceneSelectScene::GetSelectedStagePath() const { return selectedStagePath_; }
