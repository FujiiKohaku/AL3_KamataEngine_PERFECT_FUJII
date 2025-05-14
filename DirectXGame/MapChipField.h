#pragma once
#include "KamataEngine.h"

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
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

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlcokHorizontal = 100;

	MapChipDate mapChipDate_;
};
