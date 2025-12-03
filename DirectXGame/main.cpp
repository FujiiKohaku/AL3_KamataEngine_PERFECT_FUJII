#include "ClearScene.h"
#include "Fade.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"

#include "SelectScene.h"
#include <Windows.h>
using namespace KamataEngine;

//===================
// シーンの種類
//===================
enum class Scene {
	kUnknown = 0,
	kTitle,
	kSceneSelect,
	kGame,
	kGameOver,
	kClear,
};

//===================
// グローバル変数
//===================
TitleScene* titleScene = nullptr;
SceneSelectScene* selectScene = nullptr;
GameScene* gameScene = nullptr;
GameOverScene* gameoverScene = nullptr;
ClearScene* clearScene = nullptr;

Scene scene = Scene::kTitle;
Scene nextScene = Scene::kUnknown;

Fade fade;
bool isSceneChanging = false;
std::string selectedStagePath = "";

//===================
// シーン全削除関数
//===================
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
	if (selectScene) {
		delete selectScene;
		selectScene = nullptr;
	}
}

//===================
// シーン切り替え関数（フェード対応）
//===================
void ChangeScene() {

	if (!fade.IsFinished())
		return;

	if (!isSceneChanging) {
		switch (scene) {
		case Scene::kTitle:
			if (titleScene->Finished()) {
				fade.Start(Fade::Status::FadeOut, 1.0f);
				isSceneChanging = true;
				nextScene = Scene::kSceneSelect;
			}
			break;

		case Scene::kSceneSelect:
			if (selectScene->Finished()) {

				selectedStagePath = selectScene->GetSelectedStagePath();

				fade.Start(Fade::Status::FadeOut, 1.0f);
				isSceneChanging = true;
				nextScene = Scene::kGame;
			}
			break;

		case Scene::kGame:
			if (gameScene->IsGameOver()) {
				fade.Start(Fade::Status::FadeOut, 1.0f);
				isSceneChanging = true;
				nextScene = Scene::kGameOver;
			} else if (gameScene->Finished()) {
				fade.Start(Fade::Status::FadeOut, 1.0f);
				isSceneChanging = true;
				nextScene = Scene::kClear;
			}
			break;

		case Scene::kGameOver:
			if (gameoverScene->Finished()) {
				fade.Start(Fade::Status::FadeOut, 1.0f);
				isSceneChanging = true;
				nextScene = Scene::kTitle;
			}
			break;

		case Scene::kClear:
			if (clearScene->Finished()) {
				fade.Start(Fade::Status::FadeOut, 1.0f);
				isSceneChanging = true;
				nextScene = Scene::kTitle;
			}
			break;
		}
	} else {
		if (fade.IsFinished()) {
			DeleteAllScenes();

			switch (nextScene) {
			case Scene::kTitle:
				titleScene = new TitleScene;
				titleScene->Initialize();
				scene = Scene::kTitle;
				break;
			case Scene::kSceneSelect:
				selectScene = new SceneSelectScene;
				selectScene->Initialize();
				scene = Scene::kSceneSelect;
				break;
			case Scene::kGame:
				gameScene = new GameScene;
				gameScene->SetMapCsvPath(selectedStagePath);
				gameScene->Initialize();
				scene = Scene::kGame;
				break;
			case Scene::kGameOver:
				gameoverScene = new GameOverScene;
				gameoverScene->Initialize();
				scene = Scene::kGameOver;
				break;
			case Scene::kClear:
				clearScene = new ClearScene;
				clearScene->Initialize();
				scene = Scene::kClear;
				break;
			}

			fade.Start(Fade::Status::FadeIn, 1.0f);
			isSceneChanging = false;
		}
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
	case Scene::kSceneSelect:
		selectScene->Update();
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
	case Scene::kSceneSelect:
		selectScene->Draw();
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

//===================
// エントリーポイント
//===================
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"LE2C_21_フジイ_コハク_TestGame");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	fade.Initialize();
	fade.Start(Fade::Status::FadeIn, 1.0f);

	while (true) {

		if (KamataEngine::Update())
			break;

		/*
		    ImGui::Begin("Scene Controller");
		    if (ImGui::Button("Go Title")) {
		        DeleteAllScenes();
		        titleScene = new TitleScene;
		        titleScene->Initialize();
		        scene = Scene::kTitle;
		        fade.Start(Fade::Status::FadeIn, 1.0f);
		    }
		    if (ImGui::Button("Go Game")) {
		        DeleteAllScenes();
		        gameScene = new GameScene;
		        gameScene->Initialize();
		        scene = Scene::kGame;
		        fade.Start(Fade::Status::FadeIn, 1.0f);
		    }
		    if (ImGui::Button("Go GameOver")) {
		        DeleteAllScenes();
		        gameoverScene = new GameOverScene;
		        gameoverScene->Initialize();
		        scene = Scene::kGameOver;
		        fade.Start(Fade::Status::FadeIn, 1.0f);
		    }
		    if (ImGui::Button("Go ClearScene")) {
		        DeleteAllScenes();
		        clearScene = new ClearScene;
		        clearScene->Initialize();
		        scene = Scene::kClear;
		        fade.Start(Fade::Status::FadeIn, 1.0f);
		    }
		    ImGui::End();*/

		ChangeScene();
		UpdateScene();
		fade.Update();

		/*	imguiManager->End();*/

		dxCommon->PreDraw();
		DrawScene();
		fade.Draw();
		/*imguiManager->Draw();*/
		dxCommon->PostDraw();
	}

	KamataEngine::Finalize();
	DeleteAllScenes();
	return 0;
}
