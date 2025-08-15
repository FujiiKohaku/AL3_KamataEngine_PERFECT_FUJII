#include "MapChipField.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

// 内部リンケージ
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

// マップチップデータをリセット
void MapChipField::ResetMapChipData() {

	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockX);

	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockY);
	}
}

// CSV読み込み
void MapChipField::LoadMapChipData(const std::string& filePath) {
	std::ifstream file(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	ResetMapChipData();

	std::string line;
	for (uint32_t y = 0; y < kNumBlockY; ++y) {
		getline(mapChipCsv, line);
		std::istringstream line_stream(line);

		for (uint32_t x = 0; x < kNumBlockX; ++x) {
			std::string word;
			getline(line_stream, word, ',');
			if (mapChipTable.contains(word)) {
				mapChipData_.data[y][x] = mapChipTable[word];
			}
		}
	}
}

// インデックスから座標取得
Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockY - 1 - yIndex), 0); }

// タイプ取得
MapChipType MapChipField::GetmapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= kNumBlockX || yIndex >= kNumBlockY) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}
