#pragma once
#include <string>
#include "KamataEngine.h"
class SceneSelectScene {
public:
	// 初期化
	void Initialize();

	// 更新処理（入力で選択）
	void Update();

	// 描画（お好みで）
	void Draw();

	// 選択完了状態チェック
	bool Finished() const;

	// 選択されたステージCSVのパス
	const std::string& GetSelectedStagePath() const;

private:
	bool isFinished_ = false;
	std::string selectedStagePath_ = "";
};
