#pragma once
#include <cstdint>
#include <kamataEngine.h>

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

class MapChipField {

public:
private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;    // 縦方向(Y方向)のブロック
	static inline const uint32_t kNumBlockHorizontal = 100; // 横方向(X方向)のブロック
};
