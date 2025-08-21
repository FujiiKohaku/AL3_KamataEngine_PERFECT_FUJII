#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include <deque>
#include <memory>
#include <vector>

class ChunkManager {
public:
	// 初期化（モデル・カメラなどをセット）
	void Initialize(Model* blockModel, Camera* camera);

	// 更新（プレイヤーの位置に応じて古いチャンク削除/新規チャンク生成）
	void Update(const Vector3& playerPos);

	// 描画
	void Draw();

private:
	struct Chunk {
		int chunkIndex = 0;
		std::vector<std::vector<std::unique_ptr<WorldTransform>>> blocks;
	};

	// 新しいチャンクを生成
	void GenerateChunk(const std::string& filepath, int index);

	// 先頭チャンクを削除
	void DeleteChunk();

	// 保持しているチャンク
	std::deque<Chunk> chunks_;

	// 参照
	Model* modelBlock_ = nullptr;
	Camera* camera_ = nullptr;

	// 設定
	const int kChunkSizeX = 20;
	const int kChunkSizeY = 20;
	const float kBlockWidth = 1.0f;
	const float kBlockHeight = 1.0f;

	// 利用するCSVファイル一覧
	std::vector<std::string> csvFileNames_ = {
	    "Resources/floor1.csv",
	    "Resources/floor2.csv",
	    "Resources/floor3.csv",
	};
};
