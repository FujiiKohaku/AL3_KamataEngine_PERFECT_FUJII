#pragma once
#include "KamataEngine.h"

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロッ
};
struct MapChipDate {
	std::vector<std::vector<MapChipType>> data;
};
class MapChipField {
public:
private:
	// ブロックの個数
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// 1ブロックのサイズ
	static inline const uint32_t kNumBlockVertical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
	//
	MapChipDate mapChipDate_;
};
