#include "SelectScene.h"
#include "Math.h"
#include <numbers>
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
	// camera_->translation_.z = -60.0f;
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

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, camera_);

	ModelPush_ = Model::CreateFromOBJ("pushSpace", true);
	worldTransformPush_.Initialize();
	worldTransformPush_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformPush_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformPush_.translation_.z = -30.0f;
	worldTransformPush_.translation_.y = -2.0f;

	modelA_ = Model::CreateFromOBJ("A", true);
	worldTransformA_.Initialize();
	worldTransformA_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformA_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformA_.translation_.z = -30.0f;
	worldTransformA_.translation_.y = 0.0f;
	worldTransformA_.translation_.x = -6.0f;
	modelB_ = Model::CreateFromOBJ("B", true);
	worldTransformB_.Initialize();
	worldTransformB_.rotation_.x = std::numbers::pi_v<float> / 2.0f;
	worldTransformB_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformB_.translation_.z = -30.0f;
	worldTransformB_.translation_.y = 0.0f;
	worldTransformB_.translation_.x = 6.0f;
}

void SceneSelectScene::Update() {

	skydome_->Update();
	// ← 上キー
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_A)) {
		selectedIndex_--;
		if (selectedIndex_ < 0)
			selectedIndex_ = (int)stagePaths_.size() - 1;
	}

	// → 下キー
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_D)) {
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
	WorldTransformUpdate(worldTransformPush_);
	WorldTransformUpdate(worldTransformA_);
	WorldTransformUpdate(worldTransformB_);
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

	ModelPush_->Draw(worldTransformPush_, *camera_);

	skydome_->Draw();

	modelA_->Draw(worldTransformA_, *camera_);

	modelB_->Draw(worldTransformB_, *camera_);

	Model::PostDraw();
}

bool SceneSelectScene::Finished() const { return isFinished_; }

const std::string& SceneSelectScene::GetSelectedStagePath() const { return selectedStagePath_; }
