#pragma once
#include <cstdint>
#include <kamataEngine.h>

// ============================
// マップチップ関連
// ============================

// マップチップの種類
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kGoal,  // ゴール
	kCoin,  // コイン
	kSpike, // トゲzz
};

// マップ全体のデータ構造
struct MapChipData {
	std::vector<std::vector<MapChipType>> data; // [行][列]の二次元配列
};

// ============================
// マップチップフィールド本体
// ============================
class MapChipField {

public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};
	// 範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下恥
		float top;    // 上端
	};

	// === CSV読み込み ===
	void LoadMapChipCsv(const std::string& filepath);

	// === 情報取得 ===
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);               // チップ種別を取得
	KamataEngine::Vector3 GetMapChipPositionbyIndex(uint32_t xIndex, uint32_t yIndex); // インデックスから座標取得

	// === 定数ゲッター ===
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }     // 縦方向(Y方向)のブロック数
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; } // 横方向(X方向)のブロック数
	// ワールド座標positionをもとに、該当するマップチップのインデックス座標を返す
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);
	// 指定インデックスのマップチップ中心座標から、ブロック幅と高さを用いて当たり判定用の矩形(Rect)を生成して返す
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// === 内部処理 ===
	void ResetMapChipData(); // マップ全体を初期化（縦×横サイズ確保）

	// === 定数 ===
	static inline const float kBlockWidth = 1.0f;           // 1ブロックの幅
	static inline const float kBlockHeight = 1.0f;          // 1ブロックの高さ
	static inline const uint32_t kNumBlockVirtical = 20;    // 縦ブロック数
	static inline const uint32_t kNumBlockHorizontal = 100; // 横ブロック数

	// === データ ===
	MapChipData mapChipData_; // マップチップの二次元データ
};
