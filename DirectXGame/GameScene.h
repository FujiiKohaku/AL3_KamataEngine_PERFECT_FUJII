#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	//////////////////
	// 関数AL3_02_02//
	///////////////////
	GameScene();
	~GameScene();
	// 平行移動行列
	KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

	// 拡大縮小行列
	KamataEngine::Matrix4x4 Matrix4x4MakeScaleMatrix(const KamataEngine::Vector3& scale);

	// 座標変換
	KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);

	// 各軸回転行列
	KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
	KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
	KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

	// 行列の積
	KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

	// アフィン変換行列
	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);
	// ビュー行列
	KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_;
	// 自キャラ
	Player* player_ = nullptr;
	// 3Dモデルデータ(block)AL3_02_02
	KamataEngine::Model* modelBlock = nullptr;
	// ブロック用のワールドトランスフォームAL3_02_02
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// デバックカメラ有効AL3_02_02
	bool isDebugCameraActive = false;
	// デバッグカメラAL3_02_02
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
};