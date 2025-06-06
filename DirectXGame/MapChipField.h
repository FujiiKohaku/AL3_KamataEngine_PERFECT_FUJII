#pragma once
#include "KamataEngine.h"
#include <math/Vector3.h>

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロッ
};
struct MapChipDate {
	std::vector<std::vector<MapChipType>> data;
};

// クラス
class MapChipField {
public:
	// 範囲矩形 02_07_page32
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filepath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	// ゲッター
	uint32_t GetBlockWidth() { return static_cast<uint32_t>(kNumBlockHorizontal); }

	uint32_t GetBlockHeight() { return static_cast<uint32_t>(kNumBlockVirtical); }

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	// 2Dマップのインデックス位置（列・行）を表すためのデータ型AL3_02_07page22
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// AL3_02_07_page22
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	// 02_07_page33
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// ブロックの個数
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// 1ブロックのサイズ
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
	//
	MapChipDate mapChipData_;
};
