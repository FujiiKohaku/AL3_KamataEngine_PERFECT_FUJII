#include "TitleScene.h"
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
	// ImGui の表示（
	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is TitleScene!");
	ImGui::End();
};
// 描画
void TitleScene::Draw() {};