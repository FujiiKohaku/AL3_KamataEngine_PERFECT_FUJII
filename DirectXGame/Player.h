#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include <algorithm>
#include <numbers>
// 前方宣言
class MapChipField;

// 02_10 21枚目
class Enemy;

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// コンストラクタ
	Player();
	// デストラクタ
	~Player();
	// getter(02_06スライド11枚目で追加)
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// ベロシティのゲッター
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	// 外部からポインタをセットするためのセッター
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 移動入力関数AL3_02_07スライド10枚目
	void InputMove();
	// 角 02_07スライド16枚目
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		kNumCorner    // 要素数
	};
	// 02_10 10枚目 ワールド座標を取得
	Vector3 GetWorldPosition();

	// 02_10 13枚目
	AABB GetAABB();

	// 02_10 21枚目 衝突応答
	void OnCollision(const Enemy* enemy);

	// 02_12_page11
	bool IsDead() const { return isDead_; }

	// 通常行動更新
	void BehaviorRootUpdate();

	// 02_14 8枚目 攻撃行動更新
	void BehaviorAttackUpdate();
	// 02_14 16枚目 通常行動初期化
	void BehaviorRootInitialize();

	// 02_14 16枚目 攻撃行動初期化
	void BehaviorAttackInitialize();
	// 振る舞い
	//   02_14 11枚目 振るまい
	enum class Behavior {
		kUnknown,
		kRoot,   // 通常状態
		kAttack, // 攻撃中
	};
	// 攻撃フェーズ
	enum class AttackPhase {
		kUnknown,      // 未定義
		kAnticipation, // 予備動作
		kAction,       // 攻撃開始
		kRecovery,     // 攻撃終了
	};

private: // C++ではメンバ変数は特別な理由がなければprivateにする
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 速度
	KamataEngine::Vector3 velocity_ = {};

	static inline float kAccelaration_ = 0.01f;

	static inline float kAttenuation = 0.05f;

	static inline float kLimitRunSpeed = 0.3f;

	// 左右
	enum class LRDorection {
		kRight,
		kLeft,
	};

	LRDorection lrDirection_ = LRDorection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.98f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 20.0f;
	// マップチップフィールドAL3_02_07
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズAL3_02_07
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// マップとの当たり判定情報AL3_02_07p12
	struct CollisionMapInfo {
		bool isHitCeiling = false;
		bool isHitLanding = false;
		bool isHitWall = false;
		Vector3 move;
	};

	// map衝突判定AL3_02_07 p13
	void CheckMapCollision(CollisionMapInfo& info);
	// 02_07 スライド14枚目
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 角を取る関数AL3_02_07_page17
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	// AL3_02_07_page_34
	static inline const float kBlank = 0.04f;

	// 接地状態の切り替え処理02_08_page_14
	void UpdateOnGround(const CollisionMapInfo& info);

	// 接地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;

	// 02_08_page_25着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;

	// 02_08_page_21 微小な数値
	static inline const float kGroundSearchHeight = 0.06f;

	// 02_08_page_27　壁と接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);

	// デスフラグ02_12_page11
	bool isDead_ = false;

	// 02_14 11枚目 振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次のふるまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;
	// 攻撃ギミックの経過カウンター
	uint32_t attackParameter_ = 0;

	// 攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::kUnknown;
	// 02_14 26枚目 予備動作の時間
	static inline const uint32_t kAnticipationTime = 8;
	// 02_14 26枚目 前進動作の時間
	static inline const uint32_t kActionTime = 5;
	// 02_14 26枚目 余韻動作の時間
	static inline const uint32_t kRecoveryTime = 12;
	WorldTransform worldTransformAttack_;

};
