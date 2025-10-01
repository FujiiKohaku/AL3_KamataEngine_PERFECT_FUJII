#include "GameScene.h"

using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {}
// 更新
void GameScene::Update() {

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}
	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is GameScene!");
	ImGui::End();
}
// 描画
void GameScene::Draw() {}