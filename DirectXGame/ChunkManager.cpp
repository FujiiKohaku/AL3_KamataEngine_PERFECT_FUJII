#include "ChunkManager.h"
#include <cstdlib> // rand()

void ChunkManager::Initialize(Model* blockModel, Camera* camera) {
	modelBlock_ = blockModel;
	camera_ = camera;

	// 最初に3チャンク分を生成して並べる
	for (int i = 0; i < 3; i++) {
		int randomIndex = rand() % csvFileNames_.size();
		GenerateChunk(csvFileNames_[randomIndex], i);
	}
}

void ChunkManager::Update(const Vector3& playerPos) {
	if (chunks_.empty())
		return;

	// プレイヤーが最初のチャンクを通り過ぎたら削除して新規追加
	float chunkEndX = (chunks_.front().chunkIndex + 1) * (kChunkSizeX * kBlockWidth);
	if (playerPos.x > chunkEndX) {
		DeleteChunk();

		int randomIndex = rand() % csvFileNames_.size();
		int nextIndex = chunks_.back().chunkIndex + 1;
		GenerateChunk(csvFileNames_[randomIndex], nextIndex);
	}

	// 各チャンク内のワールドトランスフォーム更新
	for (auto& chunk : chunks_) {
		for (auto& line : chunk.blocks) {
			for (auto& wt : line) {
				if (wt) {
					WorldTransformUpdate(*wt);
				}
			}
		}
	}
}

void ChunkManager::Draw() {
	for (auto& chunk : chunks_) {
		// 床の描画
		for (auto& line : chunk.blocks) {
			for (auto& wt : line) {
				if (wt) {
					modelBlock_->Draw(*wt, *camera_, nullptr);
				}
			}
		}
	}
}

void ChunkManager::GenerateChunk(const std::string& filepath, int index) {
	MapChipField field;
	field.LoadMapChipCsv(filepath);

	Chunk newChunk;
	newChunk.chunkIndex = index;
	newChunk.blocks.resize(kChunkSizeY);
	for (int y = 0; y < kChunkSizeY; y++) {
		newChunk.blocks[y].resize(kChunkSizeX);
	}

	for (int y = 0; y < kChunkSizeY; ++y) {
		for (int x = 0; x < kChunkSizeX; ++x) {
			MapChipType type = field.GetMapChipTypeByIndex(x, y);
			if (type == MapChipType::kBlock) {
				auto wt = std::make_unique<WorldTransform>();
				wt->Initialize();
				wt->translation_ = field.GetMapChipPositionByIndex(x, y);
				wt->translation_.x += index * (kChunkSizeX * kBlockWidth);
				newChunk.blocks[y][x] = std::move(wt);
			}
		}
	}

	chunks_.push_back(std::move(newChunk));
}

void ChunkManager::DeleteChunk() {
	if (chunks_.empty())
		return;
	chunks_.pop_front();
}
