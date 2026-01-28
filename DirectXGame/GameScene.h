#pragma once
#include "Block.h"
#include "CameraController.h"
#include "ChaserEnemy.h"
#include "Coin.h"
#include "EnemyBase.h"
#include "EnemyFlyer.h"
#include "Fade.h"
#include "Goal.h"
#include "InhaleEffect.h"
#include "JumpHopper.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "Spike.h"
#include "StageState.h"
#include "WalkEnemy.h"
#include <numbers>
#include <vector>
class GameScene {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	GameScene();
	~GameScene();
	// シーンが終了したかのゲッター
	bool Finished() const { return finished_; }
	bool IsGameOver() const { return isGameOver_; }

	void GenerateBlocks();

	void CreateBlocksFromMap();

	void CreateGoalFromMap();

	void CreateCoinsFromMap();

	void CreateSpikesFromMap();

	void CreateEnemiesFromMap();
	void CreateJumpHoppersFromMap();
	bool IsHitPlayerEnemy(Player* player, EnemyBase* enemy);
	void SetMapCsvPath(const std::string& path) { mapCsvPath_ = path; }
	void UpdateCoins();
	bool IsNearPlayer(const Vector3& pos, float range);

	// セッター
	void SetStageState(StageState state) { stageState_ = state; }

private:
	StageState stageState_ = StageState::Tutorial;
	//============
	// 状態管理
	//============
	bool finished_ = false;
	bool isGameOver_ = false; // ←シーン終了フラグ
	//============
	// player
	//============
	KamataEngine::Model* model_ = nullptr; // 3Dモデルデータ
	KamataEngine::Camera* camera_;         // カメラ
	Player* player_ = nullptr;             // 自キャラ

	KamataEngine::Vector3 pos_;

	//---------------
	// skydome
	//---------------
	Skydome* skydome_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	//---------------
	// debugCamera
	//---------------
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	//---------------
	// block
	//---------------

	KamataEngine::Model* modelBlock_;

	KamataEngine::Model* modelBreakBlock_;
	// 変更後
	std::vector<std::vector<Block*>> blocks_;

	//---------------
	// MapChipField
	//---------------

	// マップチップフィールド
	MapChipField* mapChipField_;

	//---------------
	// カメラコントローラー
	//---------------
	CameraController* cController_;
	//---------------
	// ゴール
	//---------------
	Goal* goal_ = nullptr;
	//---------------
	// コイン
	//---------------
	std::vector<Coin*> coins_;
	Model* coinModel_ = nullptr;

	//---------------
	// とげ
	//---------------
	std::vector<Spike*> spikes_;
	//---------------
	// フェード
	//---------------
	Fade fade_;                // フェードクラス
	bool isFadingOut_ = false; // フェードアウト中かどうか

	//---------------
	// エネミー
	//---------------
	std::vector<EnemyBase*> enemies_;
	// map
	std::string mapCsvPath_ = "Resources/map/blocks.csv";

	// textureHandle
	uint32_t textureHandleExp_ = 0;

	Sprite* explanationSprite_ = nullptr;

	std::vector<JumpHopper*> jumpHoppers_;
	Model* jumpHopperModel_ = nullptr;

	uint32_t textureFullHp_ = 0;
	uint32_t textureEmptyHp_ = 0;
	Sprite* heartFull_ = nullptr;
	Sprite* heartEmpty_ = nullptr;
	std::vector<Sprite*> heartsFull_;
	std::vector<Sprite*> heartsEmpty_;

	uint32_t BgmHandle_ = 0;

	uint32_t bgmPlayHandle_ = 0;

	uint32_t AttackSEHandle_ = 0;

	uint32_t coinSEHandle_ = 0;

	// チュートリアル看板
	Model* tutorialSignModel_ = nullptr;
	WorldTransform worldTransformTutorialSign_;
};