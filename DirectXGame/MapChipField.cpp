#include "MapChipField.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

// 内部リンケージ
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank },
    {"1", MapChipType::kBlock },
    {"2", MapChipType::kSpike },
    {"3", MapChipType::kEnemy },
    {"4", MapChipType::kSpring},
};
}

// マップチップデータをリセット
void MapChipField::ResetMapChipData() {

	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);

	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

// CSV読み込み
void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	std::ifstream file(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	ResetMapChipData();

	std::string line;
	for (uint32_t y = 0; y < kNumBlockVirtical; ++y) {
		getline(mapChipCsv, line);
		std::istringstream line_stream(line);

		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			std::string word;
			getline(line_stream, word, ',');
			if (mapChipTable.contains(word)) {
				mapChipData_.data[y][x] = mapChipTable[word];
			}
		}
	}
}

// インデックスから座標取得
Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0); }

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByposition(const Vector3& position) {

	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>(position.y + kBlockHeight / 2.0f / kBlockHeight);
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 指定ブロックの中心座標を取得する
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
	Rect rect = {};
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	return rect;
}
