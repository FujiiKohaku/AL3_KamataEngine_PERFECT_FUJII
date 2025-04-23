#include "GameScene.h"
#include "ImGui.h"
using namespace KamataEngine;

// 関数はpublicより↓
// コンストラクタ
GameScene::GameScene() {}
// デストラクタ
GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

// 初期化
void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("images.jpg");

	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransForm_.Initialize();
	// カメラの初期化
	camera_.Initialize();

	// サウンドデータの読み込み
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	// 音声再生
	Audio::GetInstance()->PlayWave(soundDataHandle_);
	// 音声再生
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	// ライン描画が参照カメラを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1290, 720);
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	Audio::GetInstance()->StopWave(voiceHandle_);
}

// 更新
void GameScene::Update() {
	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	// 座標を{2,1}移動
	position.x += 2.0f;
	position.y += 1.0f;
	// 移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	// スペースキーを推した瞬間
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		// 音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}
	// デバックテキストの表示
	ImGui::Text("kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::ShowDemoWindow();
	// デバックカメラの更新
	debugCamera_->Update();
}
// 描画
void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	sprite_->Draw();

	// スプライト描画処理
	Sprite::PostDraw();

	// DirectXCommonインスタンスの取得
	// DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	// 3Dモデル描画
	model_->Draw(worldTransForm_, debugCamera_->GetCamera(), textureHandle_);
	// 3Dモデル描画後処理
	Model::PostDraw();

	// ラインを描画する
	PrimitiveDrawer::GetInstance()->DrawLine3d(
	    {
	        0,
	        0,
	        0,
	    },
	    {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
}