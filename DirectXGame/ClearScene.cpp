#include "ClearScene.h"
using namespace KamataEngine;
void ClearScene::Initialize() {}

void ClearScene::Update() {

	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is ClearScene!");
	ImGui::End();
}
void ClearScene::Draw() {}