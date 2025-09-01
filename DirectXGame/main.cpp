#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();

	// DirectXCommonインスタンスの取得
	KamataEngine::DirectXCommon* dxCommon = KamataEngine::DirectXCommon::GetInstance();

	// imGuiManagerのインスタンスを取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// ゲームシーンの生成・初期化
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新（true なら終了）
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// ゲームシーンの更新
		gameScene->Update();

		// imGui受付終了
		imguiManager->End();

		// 描画
		dxCommon->PreDraw();
		gameScene->Draw();

		// ImGui描画
		imguiManager->Draw();

		dxCommon->PostDraw();
	}

	// ゲームシーンの解放（順番に注意）
	delete gameScene;
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();
	return 0;
}
