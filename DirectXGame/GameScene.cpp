#include "GameScene.h"

using namespace KamataEngine;
// 初期化
void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");
	// 3Dモデルデータの生成
	model_ = Model::Create();
	camera_ = new Camera();
	// カメラの初期化
	camera_->Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, camera_);
	// 3Dモデルデータの生成(block)AL3_02_02
	modelBlock = Model::Create();

	// 初期化AL3_02_02
	// 要素数AL3_02_02
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック１個文の横幅AL3_02_02
	const float kBlockWidth = 2.0f;
	// 要素数を変更するAL3_02_02
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	// キューブの生成AL3_02_02
	for (uint32_t i = 0; i < kNumBlockHorizontal; ++i) {
		worldTransformBlocks_[i] = new WorldTransform();
		worldTransformBlocks_[i]->Initialize();
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		worldTransformBlocks_[i]->translation_.y = 0.0f;
	}
}
// 更新
void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	/* ブロックの更新AL3_02_02*/
	for (auto worldTransformBlock : worldTransformBlocks_) {

		worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

		worldTransformBlock->TransferMatrix();
	}
}
// 描画
void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 自キャラの描画
	model_->PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	// ブロックの描画
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		modelBlock->Draw(*worldTransformBlock, *camera_, nullptr);
	}
	model_->PostDraw();
}
// コンストラクタ
GameScene::GameScene() {}
// デストラクタ
GameScene::~GameScene() {
	// 3Dモデルデータの解散
	delete model_;
	// 自キャラの解散
	delete player_;
	/* 3Dモデルデータの解放(block)AL3_02_02*/
	for (auto worldTransFormBlock : worldTransformBlocks_) {
		delete worldTransFormBlock;
	}
	worldTransformBlocks_.clear();
}

// 平行移動行列
KamataEngine::Matrix4x4 GameScene::MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}
// 拡大縮小行列
KamataEngine::Matrix4x4 GameScene::Matrix4x4MakeScaleMatrix(const KamataEngine::Vector3& scale) {
	KamataEngine::Matrix4x4 result = {};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;
	return result;
}
// 座標変換行列
KamataEngine::Vector3 GameScene::Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix) {
	KamataEngine::Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];

	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];

	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 1.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
// X軸回転行列
KamataEngine::Matrix4x4 GameScene::MakeRotateXMatrix(float radian) {
	KamataEngine::Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 GameScene::MakeRotateYMatrix(float radian) {
	KamataEngine::Matrix4x4 result = {};

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = std::sin(radian);
	result.m[1][1] = 1.0f;
	result.m[2][0] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 GameScene::MakeRotateZMatrix(float radian) {
	KamataEngine::Matrix4x4 result = {};

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = -std::sin(radian);
	result.m[1][0] = std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 GameScene::Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
	return result;
}

// 拡大縮小・回転・平行移動行列を使ってアフィン変換行列を作る関数
KamataEngine::Matrix4x4 GameScene::MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {
	// スケーリング行列の作成
	KamataEngine::Matrix4x4 matScale = Matrix4x4MakeScaleMatrix(scale);

	KamataEngine::Matrix4x4 matRotX = MakeRotateXMatrix(rotate.x);
	KamataEngine::Matrix4x4 matRotY = MakeRotateYMatrix(rotate.y);
	KamataEngine::Matrix4x4 matRotZ = MakeRotateZMatrix(rotate.z);
	// 回転行列の合成
	KamataEngine::Matrix4x4 matRot = Multiply(Multiply(matRotZ, matRotX), matRotY);

	// 平行移動行列の作成
	KamataEngine::Matrix4x4 matTrans = MakeTranslateMatrix(translate);

	// スケーリング、回転、平行移動の合成
	KamataEngine::Matrix4x4 matTransform = Multiply(Multiply(matScale, matRot), matTrans);

	return matTransform;
}
