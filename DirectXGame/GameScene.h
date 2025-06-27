#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "Player.h"
#include "Skydome.h"
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

	void GenerateBlocks();

	// 02_10 16枚目 衝突判定と応答
	void CheckAllCollisions();
	// デスフラグのgetter
	bool IsFinished() const { return finished_;}

private:
	// 02_12 4枚目 ゲームのフェーズ（型）
	enum class Phase {
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
	};

	// 02_12 4枚目 ゲームの現在フェーズ（変数）
	Phase phase_;

	// 02_12 9枚目
	void ChangePhase();
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera camera_;
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
	// スカイドームモデル
	KamataEngine::Model* modelSkydome_ = nullptr;
	// スカイドーム
	Skydome* skydome_ = nullptr;
	// マップチップフィールド
	MapChipField* mapChipField_;
	// カメラコントローラ
	CameraController* cController_ = nullptr;

	// 02_09 10枚目 エネミークラス
	/*Enemy* enemy_ = nullptr;*/ // 02_10で削除
	// 02_09 10枚目 エネミーモデル
	KamataEngine::Model* enemy_model_ = nullptr;

	// 02_10
	std::list<Enemy*> enemies_;

	// 02_11_page_15
	DeathParticles* deathParticles_ = nullptr;
	// 02_11_page_16
	Model* deathParticleModel = nullptr;

	// ゲームのフェーズ型
	// classは名前被りがないためだって鍵付きロッカーみたいな
	//enum class Phase {
	//	kPlay,  // ゲームプレイ
	//	kDeath, // デス演出
	//};
	// ゲームの現在フェーズの変数
	//Phase phase_;

	void ChangePhese();
	// 終了フラグ
	bool finished_ = false;
};