#include "GameScene.h"
using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {
	playerModel_ = KamataEngine::Model::CreateFromOBJ("cube", true);

	camera_.Initialize();
	player_ = new Player();

	player_->Initialize(playerModel_, &camera_, {0.0f, 1.0f, 1.0f});

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
}
// 更新
void GameScene::Update() {

	player_->Update();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}
	ImGui::Begin("TitleScene Debug");
	ImGui::Text("This is GameScene!");
	ImGui::Begin("Debug");
	Vector3 pos = player_->GetWorldPosition();
	ImGui::Text("Player Pos: x=%.2f y=%.2f z=%.2f", pos.x, pos.y, pos.z);

	ImGui::End();
	// --- カメラ追従処理 ---
	Vector3 playerPos = player_->GetWorldPosition();

	camera_.translation_.x = playerPos.x;
	camera_.translation_.y = playerPos.y + 3.0f;  // 少し上から見る
	camera_.translation_.z = playerPos.z - 10.0f; // 少し後ろから
	camera_.rotation_.x = 0.3f;                   // 少し下向きに

	camera_.UpdateMatrix(); // ← カメラ行列更新を忘れずに！
}
// 描画
void GameScene::Draw() {
	KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	player_->Draw();

	Model::PostDraw();

	// グリッド表示
	for (int i = -10; i <= 10; i++) {
		// Z方向の線
		PrimitiveDrawer::GetInstance()->DrawLine3d({(float)i, 0.0f, -10.0f}, {(float)i, 0.0f, 10.0f}, {0.5f, 0.5f, 0.5f, 1.0f});

		// X方向の線
		PrimitiveDrawer::GetInstance()->DrawLine3d({-10.0f, 0.0f, (float)i}, {10.0f, 0.0f, (float)i}, {0.5f, 0.5f, 0.5f, 1.0f});
	}
}