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

	void LoadMapChipCsv(const std::string& filepath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndedx);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};
	// 範囲矩形
	struct Rect {
		float left;
		float right;
		float top;
		float bottom;
	};

	IndexSet GetMapChipIndexSetByposition(const Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// 1ブロックのサイズ
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 200;
	// 二次元配列の構造体の変数
	MapChipData mapChipData_;

};
