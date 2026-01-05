#pragma once
#include "BreakBlock.h"
#include "CameraController.h"
#include "Coin.h"
#include "EnemyBase.h"
#include "Fade.h"
#include "Goal.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "Spike.h"
#include <vector>

#include "JumpHopper.h"
#include "WalkEnemy.h"
// ゲームシーン
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

private:
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
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // 二次元配列

	//---------------
	// MapChipField
	//---------------

	// マップチップフィールド
	MapChipField* mapChipField_;

	//---------------
	// カメラコントローラー
	//---------------

	// カメラコントローラー
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
	// 壊れるブロック
	//---------------
	std::vector<BreakBlock*> breakBlocks_;
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
};