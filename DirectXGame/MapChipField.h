#pragma once
#include "KamataEngine.h"
#include "Math.h"
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {

	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	void Initialize();

	void UpDate();

	void Draw();

	void ResetMapChipData();

	void LoadMapChipData(const std::string& filepath);

	MapChipType GetmapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndedx);

	uint32_t GetNumBlockVirtical() const { return kNumBlockX; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockY; }

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockX = 20;
	static inline const uint32_t kNumBlockY = 100;
	// 二次元配列の構造体の変数
	MapChipData mapChipData_;
};
