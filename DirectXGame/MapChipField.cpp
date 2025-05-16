#include "MapChipField.h"
#include <map>
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData() {
	// マップチップデータをリセット
	mapChipDate_.data.clear();
	mapChipDate_.data.resize(kNumBlockVertical);
	for (std::vector<MapChipType>& mapChipDateLine : mapChipDate_.data) {
		mapChipDateLine.resize(kNumBlockHorizontal);
	}
}
