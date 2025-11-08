#include "ClearScene.h"
using namespace KamataEngine;
void ClearScene::Initialize() {}

void ClearScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		finished_ = true;
	}
	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is ClearScene!");
	ImGui::End();
}
void ClearScene::Draw() {}