#include "GameOverScene.h"
using namespace KamataEngine;
void GameOverScene::Initialize() {};

void GameOverScene::Update() {

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		finished_ = true;
	}
	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is GameOverScene!");
	ImGui::End();
};

void GameOverScene::Draw() {};