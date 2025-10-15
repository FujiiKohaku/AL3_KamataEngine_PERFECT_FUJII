#pragma once
#include "KamataEngine.h"
// ゲームシーン
class GameOverScene {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	// シーンが終了したかのゲッター
	bool Finished() const { return finished_; }

private:
	//============
	// 状態管理
	//============
	bool finished_ = false;


};