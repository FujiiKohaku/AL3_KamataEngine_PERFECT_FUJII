#pragma once
#include <iostream>
#include <random>
class Random {
public:
	Random() { // コンストラクタ
		// 乱数生成エンジン
		std::random_device seedGenerator;
		// メルセンヌツイスターエンジンの初期化
		randomEngine.seed(seedGenerator());
	}

private:
	// メルセンヌ。ツイッターエンジン(64bit盤)
	std::mt19937_64 randomEngine;

	float RandomNumber_;
};
