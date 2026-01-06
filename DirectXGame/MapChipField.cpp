#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>
#include <cassert>
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
    {"2", MapChipType::kGoal }, // ゴール
    {"3", MapChipType::kCoin }, // コイン
    {"4", MapChipType::kSpike}, // 壊れるブロックzz
    {"5", MapChipType::kEnemy}, // 敵
    {"6", MapChipType::kJumpPad}, // ジャンプ台
    {"7", MapChipType::kEnemyFlyer}, // 飛行型エネミー
    {"8", MapChipType::kEnemySplit}, // 分裂型エネミー
};
}

void MapChipField::ResetMapChipData() {

	// マップチップデータのリセット
	mapChipData_.data.clear();                   // 一度すべての行データをクリア
	mapChipData_.data.resize(kNumBlockVirtical); // 縦方向（行）の数を確保

	// 各行ごとに横方向のブロック数を確保する
	// → 外側のvectorが「行」、内側のvectorが「列」に相当するため、
	//    for文で各行を1つずつ取り出して「列数」をresizeしている
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal); // 横方向（列）の数を確保
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filepath) {

	// マップデータを初期化（既存の内容を消す）
	ResetMapChipData();

	// CSVファイルを開く（ifstreamのコンストラクタで同時にopen）
	std::ifstream file;
	file.open(filepath);
	assert(file.is_open()); // ファイルが開けなければエラーで停止

	// マップチップとCSV
	// ファイル内容をすべて文字列ストリームに読み込む
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();
	// ===============================================
	// CSVからマップチップデータを読み込む
	// -----------------------------------------------
	// ・外側のfor文（i）は縦方向の行を読み取る
	// ・内側のfor文（j）は横方向の列を読み取る
	// ・各セルに書かれた文字列（"block"など）を
	//   mapChipTableで対応するenum型に変換して保存する
	// ===============================================
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		std::getline(mapChipCsv, line); // CSVから1行ぶんを取得（例："block,blank,block"）

		// 1行の文字列を解析しやすいようにストリーム化
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {

			std::string word;
			std::getline(line_stream, word, ','); // カンマ区切りで1単語ずつ取得（例："block"）

			// mapChipTableにその単語が登録されていれば、対応するマップタイプを代入
			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word]; // i=0,j=0の時は左上に格納される
			}
		}
	}
}

// ============================================================
// 指定したインデックス（x, y）のマップチップ種別を取得する関数
// ------------------------------------------------------------
// ・xIndex : 横方向のインデックス（列）
// ・yIndex : 縦方向のインデックス（行）
//
// 範囲外のインデックスを指定された場合、
// 配列アクセスによるクラッシュを防ぐために
// 空白チップ（kBlank）を返す。
// ============================================================
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {

	if (xIndex >= kNumBlockHorizontal) {
		return MapChipType::kBlank;
	}

	if (yIndex >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}


// マップ配列のインデックスからワールド座標を計算
KamataEngine::Vector3 MapChipField::GetMapChipPositionbyIndex(uint32_t xIndex, uint32_t yIndex) {

	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);
}

// ワールド座標positionをもとに、該当するマップチップのインデックス座標を返す
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet = {};

	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>(position.y + kBlockHeight / 2.0f / kBlockHeight);

	return indexSet;
}

// 指定インデックスのマップチップ中心座標から、ブロック幅と高さを用いて当たり判定用の矩形(Rect)を生成して返す
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	// 指定ブロックの中心座標を取得する
	KamataEngine::Vector3 center = GetMapChipPositionbyIndex(xIndex, yIndex);
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}
