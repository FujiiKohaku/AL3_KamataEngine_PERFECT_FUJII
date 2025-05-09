#pragma once
#include "Function.h"
#include "KamataEngine.h"
class WorldRowFunction {
public:
	WorldRowFunction();
	~WorldRowFunction();

	static void MakeAffinTransFerMatrix(KamataEngine::WorldTransform& worldTransform);

private:
};
