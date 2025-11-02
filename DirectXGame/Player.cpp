#include "Player.h"
#include <cassert>
#include <numbers>
using namespace KamataEngine;

// 初期化
void Player::Initialize(Model* model, Camera* camera, const Vector3 position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

// コンストラクタ
Player::Player() {}
// デストラクタ
Player::~Player() {}
// 更新
void Player::Update() {

	// 移動処理①
	UpdateMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// ===== ジャンプ処理 =====
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onGround_) {
		velocity_.y = kJumpPower;
		onGround_ = false;
	}

	// ===== 重力 =====
	velocity_.y -= kGravity;

	// ===== 座標更新 =====
	worldTransform_.translation_ += velocity_; // ← 位置はvelocityでまとめて更新！

	// ===== 地面との判定 =====
	if (worldTransform_.translation_.y <= kGroundY) {
		worldTransform_.translation_.y = kGroundY;
		velocity_.y = 0;
		onGround_ = true;
	}

	// ===== 向き変更 =====
	if (facingRight_) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	// ===== 行列更新 =====
	WorldTransformUpdate(worldTransform_);

	// --- ImGui デバッグ表示 ---
	ImGui::Begin("Player Debug");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", velocity_.x, velocity_.y, velocity_.z);
	ImGui::End();
}

// 描画
void Player::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Player::UpdateMove() {

	velocity_.x = 0.0f;

	if (Input::GetInstance()->PushKey(DIK_D)) { // →キー
		velocity_.x += kMoveSpeed;
		facingRight_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) { // ←キー
		velocity_.x -= kMoveSpeed;
		facingRight_ = false;
	}
}

// マップ衝突判定
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

// マップ衝突判定上
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 移動後の4つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew; // Vector3型の要素を4つ持つ固定長配列（角の座標を格納）
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i)); // 移動後のプレイヤー中心を基準に、4つの角の座標を計算して格納する
	}
}
// マップ衝突判定下
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {}
// マップ衝突判定右
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {}
// 　マップ衝突判定左
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {}
// 指定したオブジェクトの角を取得する関数
KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
	KamataEngine::Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, // kLeftTop
	};
	// 指定した角(corner)に対応するオフセットをoffsetTableから取得し、中心座標(center)に加算して返す
	return center + offsetTable[static_cast<uint32_t>(corner)];
}
