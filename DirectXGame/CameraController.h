#pragma once
#include "KamataEngine.h"
class CameraController {
public:
	void Initialize();

	void Upadate();

private:
	// カメラ
	KamataEngine::Camera *camera_=nullptr;
};
