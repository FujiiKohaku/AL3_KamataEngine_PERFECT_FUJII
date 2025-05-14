#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>
void MapChipField::Initialize() {}

void MapChipField::Update() {}

void MapChipField::Draw() {}

MapChipField::MapChipField() {}

MapChipField::~MapChipField() {}

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
}

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}
