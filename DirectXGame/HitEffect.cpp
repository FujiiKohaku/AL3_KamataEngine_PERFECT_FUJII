#include "HitEffect.h"
// 初期化
void Initialize();
// 更新
void Update();
// 描画
void Draw();

// 静的メンバ変数の実体
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;
