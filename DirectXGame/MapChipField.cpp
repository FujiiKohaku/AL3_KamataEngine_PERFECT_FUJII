#include "MapChipField.h"
#include "CameraController.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

// ==============================
// 内部リンケージ（CSV文字 → MapChipType変換テーブル）
// ==============================
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

// ==============================
// マップチップデータを初期化（空データにリセット）
// ==============================
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);

	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

// ==============================
// マップチップCSV読み込み
// ==============================
void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// ファイルを開く
	std::ifstream file(filePath);
	assert(file.is_open());

	// ファイル内容を文字列ストリームにコピー
	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	// マップデータリセット
	ResetMapChipData();

	std::string line;

	// CSVを1行ずつ読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		getline(mapChipCsv, line);

		// 1行分を解析用ストリームへ
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

// ==============================
// 座標 → マップチップインデックス
// ==============================
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2.0f) / kBlockHeight);

	return indexSet;
}

// ==============================
// インデックス → マップチップ矩形範囲
// ==============================
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockWidth / 2.0f;
	rect.top = center.y + kBlockWidth / 2.0f;
	return rect;
}

// ==============================
// インデックス → マップチップ中心座標
// ==============================
KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);
}

// ==============================
// インデックス → マップチップ種別
// ==============================
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 範囲外なら空白
	if (xIndex >= kNumBlockHorizontal) {
		return MapChipType::kBlank;
	}
	if (yIndex >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}
