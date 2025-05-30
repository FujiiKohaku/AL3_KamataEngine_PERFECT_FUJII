#include "WorldRowFunction.h"
#include "Math.h"

WorldRowFunction::WorldRowFunction() {}

WorldRowFunction::~WorldRowFunction() {}

void WorldRowFunction::MakeAffinTransFerMatrix(KamataEngine::WorldTransform& worldTransform) {

	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.TransferMatrix();
}
