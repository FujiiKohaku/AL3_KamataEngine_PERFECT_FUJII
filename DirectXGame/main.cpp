#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
// タイトルシーンのポインタ（初期化はまだ）02_12_page21
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
// 02_12 25枚目(Scene sceneまで)
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};
// 現在シーン（型）
Scene scene = Scene::kUnknown;
// 02_12 29枚//シーンを返る関数まとめます
void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		// 02_12 30枚目
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}
// アップデート関数スイッチ文で管理するため関数にまとめます
void UpdateScene() {
	switch (scene) {
	case Scene::kUnknown:
		break;
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	default:
		break;
	}
}
// ドロウ関数をスイッチ文で制御するために関数でまとめる
void DrawScene() {

	switch (scene) {
	case Scene::kUnknown:
		break;
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	default:
		break;
	}
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();

	// DirectXCommonインスタスの取得
	KamataEngine::DirectXCommon* dxCommon = KamataEngine::DirectXCommon::GetInstance();
	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		// ImGui受付開始
		imguiManager->Begin();

		// シーン切り替え
		ChangeScene();

		// シーン更新
		UpdateScene();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		//軸？
		AxisIndicator::GetInstance()->Draw();
		// プリミティブ描画のリセット
		PrimitiveDrawer::GetInstance()->Reset();
		// シーンの描画
		DrawScene(); // 02_12 33枚目で追加
		             // ImGui描画
		imguiManager->Draw();
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}
	// ゲームシーンの解放
	// タイトルシーンの解放
	// nullptrの代入
	gameScene = nullptr;
	titleScene = nullptr;
	delete gameScene;
	delete titleScene;
	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
