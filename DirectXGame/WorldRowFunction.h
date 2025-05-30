#pragma once
#include "KamataEngine.h"
#include "Math.h"
class WorldRowFunction {
public:
	WorldRowFunction();
	~WorldRowFunction();

	static void MakeAffinTransFerMatrix(KamataEngine::WorldTransform& worldTransform);

private:
};
