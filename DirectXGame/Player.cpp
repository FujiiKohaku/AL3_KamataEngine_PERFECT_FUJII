#define NOMINMAX
#include "Player.h"
#include "WorldRowFunction.h"
#include "imgui.h"
#include <cassert>

using namespace KamataEngine;
// 初期化
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// 引数の内容を記録
	camera_ = camera;

	// テクスチャハンドル

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

#pragma region 移動処理関数
// 移動処理関数AL3_02_07　10ページ
void Player::InputMove() {

	// 移動入力（左右）
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDorection::kRight) {
					lrDirection_ = LRDorection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTrun;
				}
				acceleration.x += kAccelaration_;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDorection::kLeft) {
					lrDirection_ = LRDorection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.1f;
				}
				acceleration.x -= kAccelaration_;
			}

			// 速度に加算
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 非入力時は移動減衰を×
			velocity_.x *= (1.0f - kAccelaration_);
		}
		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration / 60, 0);
		}

	} else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}
#pragma endregion

#pragma region マップ衝突判定AL3_02_07
// AL02_03_page14
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

// マップ上方向衝突判定AL3_02_07_page19
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 移動後の4つの角の座標AL3_02_07_page19
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	// マップ衝突上方向AL3_02_07_page20
	// 上昇あり？
	if (info.move.y <= 0) {
		return;
	}
	MapChipType mapChipType;
	// 真上の当たり判定を行うAL3_02_07_page27
	bool hit = false;
	// 左上点の判定AL3_02_07_page28
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定AL3_02_-07_page_28
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒットAL3_02_07_page_34
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったっことを記録する
		info.isHitCeiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) { info; }
void Player::CheckMapCollisionRight(CollisionMapInfo& info) { info; }
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) { info; }

// 中心座標 center を基準に、プレイヤーの四隅のどこかの位置を返す AL3_02_07_page18
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

#pragma endregion

// 更新
void Player::Update() {
	// 移動処理が中に入っている関数AL3_02_07 p10
	Player::InputMove();

	// 位置に加算（移動）
	worldTransform_.translation_ += velocity_;

	bool landing = false;
	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に以降
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に以降
			onGround_ = true;
		}
	}
	// 旋回制御
	{
		if (turnTimer_ > 0.0f) {
			// タイマーを進める
			turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

			float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

			// 状況に応じた角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, 1.0f - turnTimer_ / kTimeTrun);
		}

		// 行列更新
		//
		//
		//
		// ここより上に処理書いて―
		WorldRowFunction::MakeAffinTransFerMatrix(worldTransform_);
	}

	// 衝突判定を初期化AL3_02_07page13
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値にコピーAL3_02_07page13
	collisionMapInfo.move = velocity_;
	// マップ衝突チェックAL3_02_07page13
	CheckMapCollision(collisionMapInfo);
	// 移動02_07_page_36
	worldTransform_.translation_ += collisionMapInfo.move;
	// 天井接触による落下開始(02_07 スライド38枚目)
	if (collisionMapInfo.isHitCeiling) {
		velocity_.y = 0;
	}
}
// 描画
void Player::Draw() {

	model_->Draw(worldTransform_, *camera_);

	/////
}

// コンストラクタ
Player::Player() {}
// デストラクタ
Player::~Player() {}
