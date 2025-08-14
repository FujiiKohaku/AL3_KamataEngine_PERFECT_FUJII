#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

// ====== シーン用グローバル ======
TitleScene* titleScene = nullptr; // タイトルシーンの実体（必要な時だけnew）
GameScene* gameScene = nullptr;   // ゲームシーンの実体（必要な時だけnew）

// シーン種別
enum class Scene {
	kUnknown = 0, // 未定（初期化中など）
	kTitle,       // タイトル
	kGame,        // ゲーム本編
};

// 現在のシーン
Scene scene = Scene::kUnknown;

// ====== シーン切り替え（終了フラグを見て次へ） ======
void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		// titleSceneが存在していて、かつ終了要求されたらゲームへ
		if (titleScene && titleScene->IsFinished()) {
			scene = Scene::kGame;

			delete titleScene;    // 先に解放
			titleScene = nullptr; // ダングリング防止

			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;

	case Scene::kGame:
		// gameSceneが存在していて、かつ終了要求されたらタイトルへ
		if (gameScene && gameScene->IsFinished()) {
			scene = Scene::kTitle;

			delete gameScene; // 先に解放
			gameScene = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;

	case Scene::kUnknown:
	default:
		// ここは基本来ない。初期化直後にkTitleへ遷移する想定。
		break;
	}
}

// ====== シーン更新 ======
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene) {
			titleScene->Update();
		}
		break;

	case Scene::kGame:
		if (gameScene) {
			gameScene->Update();
		}
		break;

	case Scene::kUnknown:
	default:
		// 何もしない
		break;
	}
}

// ====== シーン描画 ======
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene) {
			titleScene->Draw();
		}
		break;

	case Scene::kGame:
		if (gameScene) {
			gameScene->Draw();
		}
		break;

	case Scene::kUnknown:
	default:
		// 何もしない
		break;
	}
}

// ====== WinMain（エントリポイント） ======
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジン初期化
	KamataEngine::Initialize();

	// 共通インスタンス取得
	KamataEngine::DirectXCommon* dxCommon = KamataEngine::DirectXCommon::GetInstance();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// 最初はタイトルから
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// ====== メインループ ======
	while (true) {

		// エンジン更新（trueなら終了要求）
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui 入力受付開始
		imguiManager->Begin();

		// 必要ならシーン切替
		ChangeScene();

		// シーン更新
		UpdateScene();

		// ImGui 入力受付終了（ここでウィジェット確定）
		imguiManager->End();

		// ====== 描画フェーズ ======
		dxCommon->PreDraw();

		// 便利描画（座標軸/プリミティブ）
		AxisIndicator::GetInstance()->Draw();
		PrimitiveDrawer::GetInstance()->Reset();

		// シーン描画
		DrawScene();

		// ImGui描画（※二重呼び出しは不要）
		imguiManager->Draw();

		dxCommon->PostDraw();
	}

	// ====== 終了処理（delete → nullptrの順） ======
	if (gameScene) {
		delete gameScene;
		gameScene = nullptr;
	}
	if (titleScene) {
		delete titleScene;
		titleScene = nullptr;
	}

	// エンジン終了
	KamataEngine::Finalize();
	return 0;
}
