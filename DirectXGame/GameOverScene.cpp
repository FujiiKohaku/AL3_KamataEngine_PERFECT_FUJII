#include "GameOverScene.h"
#include "imgui.h" 

using namespace KamataEngine;

void GameOverScene::Initialize() {};

void GameOverScene::Update() {

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}


};

void GameOverScene::Draw() {};