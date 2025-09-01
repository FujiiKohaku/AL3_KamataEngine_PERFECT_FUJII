#pragma once
#include "KamataEngine.h"
#include <math/Vector3.h>
#include <vector>

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

// マップデータ
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

// クラス
class MapChipField {
public:
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filepath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	// ゲッター
	uint32_t GetBlockWidth() const { return static_cast<uint32_t>(kNumBlockHorizontal); }
	uint32_t GetBlockHeight() const { return static_cast<uint32_t>(kNumBlockVirtical); }

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータ
	MapChipData mapChipData_;
};
