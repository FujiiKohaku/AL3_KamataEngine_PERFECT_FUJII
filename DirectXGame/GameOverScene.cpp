#include "GameOverScene.h"

void GameOverScene::Initialize() {};

void GameOverScene::Update() {
	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is GameOverScene!");
	ImGui::End();

};

void GameOverScene::Draw() {};