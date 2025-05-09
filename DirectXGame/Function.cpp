#include "Function.h"

Function::Function() {
	}

Function::~Function() {}



// 平行移動行列
KamataEngine::Matrix4x4 Function::MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}
// 拡大縮小行列
KamataEngine::Matrix4x4 Function::Matrix4x4MakeScaleMatrix(const KamataEngine::Vector3& scale) {
	KamataEngine::Matrix4x4 result = {};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;
	return result;
}
// 座標変換行列
KamataEngine::Vector3 Function::Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix) {
	KamataEngine::Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];

	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];

	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 1.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
// X軸回転行列
KamataEngine::Matrix4x4 Function::MakeRotateXMatrix(float radian) {
	KamataEngine::Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 Function::MakeRotateYMatrix(float radian) {
	KamataEngine::Matrix4x4 result = {};

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = std::sin(radian);
	result.m[1][1] = 1.0f;
	result.m[2][0] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 Function::MakeRotateZMatrix(float radian) {
	KamataEngine::Matrix4x4 result = {};

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = -std::sin(radian);
	result.m[1][0] = std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
}

KamataEngine::Matrix4x4 Function::Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	KamataEngine::Matrix4x4 result{};
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
	return result;
}

// 拡大縮小・回転・平行移動行列を使ってアフィン変換行列を作る関数
KamataEngine::Matrix4x4 Function::MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {
	// スケーリング行列の作成
	KamataEngine::Matrix4x4 matScale = Matrix4x4MakeScaleMatrix(scale);

	KamataEngine::Matrix4x4 matRotX = MakeRotateXMatrix(rotate.x);
	KamataEngine::Matrix4x4 matRotY = MakeRotateYMatrix(rotate.y);
	KamataEngine::Matrix4x4 matRotZ = MakeRotateZMatrix(rotate.z);
	// 回転行列の合成
	KamataEngine::Matrix4x4 matRot = Multiply(Multiply(matRotZ, matRotX), matRotY);

	// 平行移動行列の作成
	KamataEngine::Matrix4x4 matTrans = MakeTranslateMatrix(translate);

	// スケーリング、回転、平行移動の合成
	KamataEngine::Matrix4x4 matTransform = Multiply(Multiply(matScale, matRot), matTrans);

	return matTransform;
}

KamataEngine::Matrix4x4 Function::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	KamataEngine::Matrix4x4  m = {};

	// 行0：X方向スケーリングと移動
	m.m[0][0] = width / 2.0f;
	m.m[3][0] = left + width / 2.0f;
	m.m[1][1] = -height / 2.0f;
	m.m[3][1] = top + height / 2.0f;
	m.m[2][2] = maxDepth - minDepth;
	m.m[2][3] = minDepth;
	m.m[3][3] = 1.0f;

	return m;
}
