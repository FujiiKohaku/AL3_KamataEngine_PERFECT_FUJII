#pragma once
#include "KamataEngine.h"

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロッ
};

struct MapChipDate {
	std::vector<std::vector<MapChipType>> date;
};

class MapChipField {
public:
	// 初期化
	void Initialize();
	// KamataEngine::Model *model, uint32_t textureHandle,KamataEngine::Camera *camera
	//  更新
	void Update();
	// 描画
	void Draw();
	// コンストラクタ
	MapChipField();
	// デストラクタ
	~MapChipField();

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filepath);

	MapChipType GetMapChipTypeIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetBlockWidth() { return static_cast<uint32_t>(kBlockWidth); }

	uint32_t GetBlockHeight() { return static_cast<uint32_t>(kBlockHeight); }

	void GenerateBlocks();

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlcokHorizontal = 100;

	MapChipDate mapChipDate_;
};
