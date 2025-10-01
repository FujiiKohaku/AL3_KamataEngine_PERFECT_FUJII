#include "ClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
using namespace KamataEngine; // これ書いておくとkamataEngine::書かなくてよい

// シーンの種類
enum class Scene {
	kunknown = 0,
	kTitle,
	kGame,
	kGameOver,
	kClear,

};
//=============
// シーンの宣言
//=============
// タイトルシーン
TitleScene* titleScene = nullptr;

// ゲームシーン
GameScene* gameScene = nullptr;

// ゲームオーバーシーン
GameOverScene* gameoverScene = nullptr;

// クリアシーン
ClearScene* clearScene = nullptr;
// 初期化で現在のタイトルシーンを固定
Scene scene = Scene::kTitle;

//===================
// シーン切り替え関数
//===================

void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->Finished()) {
			// タイトルシーンからゲームシーンに
			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene;
			gameScene->Initialize();
			scene = Scene::kGame;
		}
		break;

	case Scene::kGame:

		if (gameScene->Finished()) {

			delete gameScene;
			gameScene = nullptr;

			gameoverScene = new GameOverScene;
			gameoverScene->Initialize();
			scene = Scene::kGameOver;
		}
		break;

	case Scene::kGameOver:
		if (gameoverScene->Finished()) {

			delete gameoverScene;
			gameoverScene = nullptr;

			clearScene = new ClearScene;
			clearScene->Initialize();
			scene = Scene::kClear;
		}
		break;
	case Scene::kClear:
		break;
	}
}
//===================
// シーン更新関数
//===================
void UpdateScene() {

	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();

		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kGameOver:
		gameoverScene->Update();
		break;
	case Scene::kClear:
		clearScene->Update();
		break;
	default:
		break;
	}
}
//===================
// シーン描画関数
//===================
void DrawScene() {
	switch (scene) {

	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kGameOver:
		gameoverScene->Draw();
		break;
	case Scene::kClear:
		clearScene->Draw();
		break;
	default:
		break;
	}
}
// ==============
// シーン全削除関数
// ==============

void DeleteAllScenes() {
	if (titleScene) {
		delete titleScene;
		titleScene = nullptr;
	}
	if (gameScene) {
		delete gameScene;
		gameScene = nullptr;
	}
	if (gameoverScene) {
		delete gameoverScene;
		gameoverScene = nullptr;
	}
	if (clearScene) {
		delete clearScene;
		clearScene = nullptr;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();

	// DirectXCommonインスタスの取得
	DirectXCommon* dxComon = DirectXCommon::GetInstance();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// 最初はタイトルを設定
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {

			break;
		}
		//-----------
		// 更新処理
		//-----------

		// ImGui受付開始
		imguiManager->Begin();
		ImGui::Begin("Scene Controller");
		if (ImGui::Button("Go Title")) {

			DeleteAllScenes();

			titleScene = new TitleScene;
			titleScene->Initialize();
			scene = Scene::kTitle;
		}
		if (ImGui::Button("Go Game")) {

		DeleteAllScenes();

			gameScene = new GameScene;
			gameScene->Initialize();
			scene = Scene::kGame;
		}
		if (ImGui::Button("Go GameOver")) {

		DeleteAllScenes();

			gameoverScene = new GameOverScene;
			gameoverScene->Initialize();

			scene = Scene::kGameOver;
		}
		if (ImGui::Button("Go ClearScene")) {
			DeleteAllScenes();

			clearScene = new ClearScene;
			clearScene->Initialize();

			scene = Scene::kClear;
		}

		// シーン遷移関数
		ChangeScene();

		// シーン更新関数
		UpdateScene();

		// ImGui受付終了
		imguiManager->End();

		//-----------
		// 描画処理
		//-----------
		dxComon->PreDraw();

		// シーン描画
		DrawScene();

		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxComon->PostDraw();
	}
	// エンジンの終了処理
	KamataEngine::Finalize();
	// ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;
	return 0;
}
