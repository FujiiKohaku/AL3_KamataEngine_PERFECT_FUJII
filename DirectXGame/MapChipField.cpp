#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}
void MapChipField::ResetMapChipData() { // 新しいマップを読み込む前の準備名前紛らわしいね
	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockX);
	for (std::vector<MapChipType>& mapChipDataline : mapChipData_.data) {
		mapChipDataline.resize(kNumBlockY);
	}
}

void MapChipField::LoadMapChipData(const std::string& filepath) {

	// マップチップデータのサイズを取得
	ResetMapChipData();
	// ファイルを開く
	std::ifstream file;
	file.open(filepath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockX; ++i) {

		std::string line;

		std::getline(mapChipCsv, line);

		// １行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockY; ++j) {
			std::string word;
			std::getline(line_stream, word, ',');
			if (mapChipTable.contains(word)) {

				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetmapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockX - 1 < xIndex) {
		return MapChipType::kBlank;
	}

	if (yIndex < 0 || kNumBlockY - 1, yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndedx) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockY - 1 - yIndedx), 0); }
