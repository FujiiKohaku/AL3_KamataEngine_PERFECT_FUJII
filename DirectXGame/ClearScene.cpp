#include "ClearScene.h"
using namespace KamataEngine;
void ClearScene::Initialize() {}

void ClearScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		finished_ = true;
	}

}
void ClearScene::Draw() {}