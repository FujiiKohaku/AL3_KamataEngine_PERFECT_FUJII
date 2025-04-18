#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine; // これ書いておくとkamataEngine::書かなくてよい

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();

	// DirectXCommonインスタスの取得
	DirectXCommon* dxComon = DirectXCommon::GetInstance();
	// ゲームシーンのインスタンス生成//
	GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();
	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {

			break;
		}
		// ImGui受付開始
		imguiManager->Begin();
		// ゲームシーンの更新
		gameScene->Update();
		// ImGui受付終了
		imguiManager->End();
		// 確認//////
		//  描画開始
		dxComon->PreDraw();
		// ここに描画処理を記述する

		// ゲームシーンの描画
		gameScene->Draw();
		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxComon->PostDraw();
	}
	// nullptrの代入
	gameScene = nullptr; // 終了処理
	// ゲームシーンの解放
	delete gameScene;
	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
