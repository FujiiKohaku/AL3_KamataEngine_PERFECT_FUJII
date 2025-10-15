#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <algorithm>
#include <array>
#include <numbers>
// 前方宣言
class MapChipField;
class Enemy; // 02_10 21枚目

//==================================================
// プレイヤークラス
//==================================================
class Player {
public:
	// コンストラクタ / デストラクタ
	Player();
	~Player();

	// 初期化
	void Initialize(KamataEngine::Model* modelNormal, KamataEngine::Model* modelRolling, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	//==================================================
	// Getter / Setter
	//==================================================
	// ワールド変換を返す（描画などに利用）
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 現在の速度を返す
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	// マップチップフィールドを外部から設定
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	//==================================================
	// 入力処理
	//==================================================
	// 移動入力処理（AL3_02_07 スライド10枚目）
	void InputMove();

	enum class PlayerState { Normal, Rolling, Attack };
	PlayerState state_ = PlayerState::Normal;
	bool IsDead() const { return isDead_; }

	// 攻撃フェーズ
	enum class AttackPhase {
		kUnknown,
		kAnticipation, // 溜め
		kAction,       // 突進
		kRecovery      // 余韻
	};

	AttackPhase attackPhase_ = AttackPhase::kUnknown;
	uint32_t attackTimer_ = 0;

	// 攻撃処理
	void UpdateAttack();
	// 攻撃中かどうかを返す
	bool IsAttack() const { return state_ == PlayerState::Attack; }

private:
	//==================================================
	// メンバ変数
	//==================================================
	// Transform / Rendering
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelRollling_ = nullptr;
	uint32_t textureHandle_ = 0u;
	KamataEngine::Camera* camera_ = nullptr;

	// 物理
	KamataEngine::Vector3 velocity_ = {};  // 現在の速度
	bool onGround_ = true;                 // 接地フラグ
	MapChipField* mapChipField_ = nullptr; // マップフィールド

	//==================================================
	// キャラ挙動用パラメータ
	//==================================================
	// 移動パラメータ
	static inline float kAccelaration_ = 0.01f; // 加速度
	static inline float kAttenuation = 0.05f;   // 減衰率
	static inline float kLimitRunSpeed = 0.2f;  // 最大走行速度

	// ジャンプ / 重力
	static inline const float kGravityAcceleration = 0.98f; // 重力加速度
	static inline const float kLimitFallSpeed = 0.5f;       // 最大落下速度
	static inline const float kJumpAcceleration = 20.0f;    // ジャンプ初速

	// プレイヤー
	float kWidth = 0.8f;                      // 横幅
	float kHeight = 1.5f;                     // 高さ
	static inline const float kBlank = 0.04f; // 判定の余白

	// 接地 / 壁パラメータ
	static inline const float kAttenuationLanding = 0.0f;  // 着地時減衰
	static inline const float kAttenuationWall = 0.2f;     // 壁接触時減衰
	static inline const float kGroundSearchHeight = 0.06f; // 微小高さ

	// 旋回関連
	enum class LRDorection { kRight, kLeft };
	LRDorection lrDirection_ = LRDorection::kRight;
	float turnFirstRotationY_ = 0.0f;           // 旋回開始角度
	float turnTimer_ = 0.0f;                    // 旋回タイマー
	static inline const float kTimeTrun = 0.3f; // 旋回にかける時間（秒）

	bool isDead_ = false;

	float dustEmitTimer_;
	const float kJumpInitialVelocity = 0.25f; // 最低ジャンプ高さ
	const float kJumpBoost = 0.35f;           // 長押し補助の強さ

	int jumpCount_ = 0;          // 今何回ジャンプしたか
	const int kMaxJumpCount = 2; // 最大ジャンプ回数（二段ジャンプなら2）

	uint32_t soundHandleAttack = 0;

	uint32_t soundHandleDeath = 0;

	uint32_t soundHandleSpring = 0;

	uint32_t soundHandleChange = 0;
};