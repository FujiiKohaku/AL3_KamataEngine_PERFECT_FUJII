#pragma once
#include "KamataEngine.h"
class Function {
public:
	// コンストラクタ
	Function();
	~Function();
	// 平行移動行列
	static KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

	// 拡大縮小行列
	static KamataEngine::Matrix4x4 Matrix4x4MakeScaleMatrix(const KamataEngine::Vector3& scale);

	// 座標変換
	static KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);

	// 各軸回転行列
	static KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
	static KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
	static KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

	// 行列の積
	static KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

	// アフィン変換行列
	static KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);
	// ビュー行列
	static KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

private:
};
