#include "TitleScene.h"
#include "imgui.h"
using namespace KamataEngine; // これ書いておくとkamataEngine::書かなくてよい
// 初期化
void TitleScene::Initialize() {

};
// 更新
void TitleScene::Update() {
	// スペースを押したら終了フラグがオンになる
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		finished_ = true;
	}

};
// 描画
void TitleScene::Draw() {};