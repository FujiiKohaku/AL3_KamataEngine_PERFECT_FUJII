#include "MapChipField.h"
#include "vector"
#include <fstream>
#include <map>
#include <sstream>
void MapChipField::Initialize() {}

void MapChipField::Update() {}

void MapChipField::Draw() {}

MapChipField::MapChipField() {}

MapChipField::~MapChipField() {}

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData() {

	// マップチップデータをリセット
	mapChipDate_.date.clear();
	mapChipDate_.date.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDateLine : mapChipDate_.date) {
		mapChipDateLine.resize(kNumBlcokHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filepath) {
	// マップチップデータをリセット
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
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		std::getline(mapChipCsv, line);

		// １行文の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlcokHorizontal; ++j) {

			std::string word;
			std::getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipDate_.date[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeIndex(uint32_t xIndex, uint32_t yIndex) {

	if (xIndex < 0 || kNumBlcokHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipDate_.date[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {

	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);

}
