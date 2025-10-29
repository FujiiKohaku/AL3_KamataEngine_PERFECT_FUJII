#pragma once
#include <cstdint>
#include <kamataEngine.h>

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};
// MapChipDataの構造体
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

public:
	// CSV読み込み関数
	void LoadMapChipCsv(const std::string& filepath);

	// Indexからマップチップの種類を判別する関数
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	// indexから座標を計算する関数
	KamataEngine::Vector3 GetMapChipPositionbyIndex(uint32_t xIndex, uint32_t yIndex);
	// 縦方向(Y方向)のブロック数を取得
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }

	// 横方向(X方向)のブロック数を取得
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

private:
	// マップ全体を初期化して、縦×横サイズ分のマップチップを確保する
	void ResetMapChipData();
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;    // 縦方向(Y方向)のブロック
	static inline const uint32_t kNumBlockHorizontal = 100; // 横方向(X方向)のブロック

	// マップチップデータ
	MapChipData mapChipData_;
};
