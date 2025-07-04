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
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行うAL3_02_07_page27
	bool hit = false;
	// 左上点の判定AL3_02_07_page28
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// 今いるマスの1個下のマスが何か（ブロックかどうか）」を調べる02_08_page_8
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	// ブロックだったらhit=true02_08_page_8
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 02_08_page_8
	// if (mapChipType == MapChipType::kBlock) {
	//	hit = true;
	// }
	//  右上点の判定AL3_02_-07_page_28
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// 今いるマスの1個下のマスが何か（ブロックかどうか）」を調べる02_08_page_8
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	// ブロックだったらhit=true02_08_page_8
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	/*if (mapChipType == MapChipType::kBlock) {
	    hit = true;
	}*/
	// プレイヤーの上端（頭）の座標
	Vector3 playerTopPos = worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0);
	// ブロックにヒットAL3_02_07_page_34
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// 現在座標が壁の外化判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(playerTopPos);

		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったっことを記録する
		info.isHitCeiling = true;
	}
}
// マップ下当たり判定02_08
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	// 下降あり02_08_page_7
	if (info.move.y >= 0) {
		return;
	}
	// 移動後の４つの過度の座標の計算02_08_page_7
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	// MapChipType型の変数mapChipTypeを作る02_08_page_8
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	// 左下の判定
	// どこのマスにいるかを記録するための変数を作る
	MapChipField::IndexSet indexSet;
	// positionsNew[kRightBottom] は「左下の座標（プレイヤーの右下の角）」
	// それを渡して「左下の角が、マップのどのマスにいるか？」を indexSet に入れる
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	// indexSet でわかったマスの中身（タイプ）を調べます
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// 今いるマスの1個上のマスが何か（ブロックかどうか）」を調べる02_08_page_8
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	// ブロックだったらhit=true02_08_page_8
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 中身がブロックだったらぶつかったと判定してhitをtrueにする//02_08_page_32消しました
	// if (mapChipType == MapChipType::kBlock) {
	//	hit = true;
	//}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// 今いるマスの1個上のマスが何か（ブロックかどうか）」を調べる
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	// ブロックだったらhit=true
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 02_08_page_32消しました
	// if (mapChipType == MapChipType::kBlock) {
	//	hit = true;
	// }
	// または、プレイヤーの下端（足元）の座標02_08_page39
	Vector3 playerBottomPos = worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0);
	// 02_08_page_11ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(playerBottomPos);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 地面に当たったことを記録する
			info.isHitLanding = true;
		}
	}
}

// マップ下当たり判定02_08_page_25
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0)
		return;

	// 移動後の四隅の位置を取得
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;

	// 右上・右下の1マス右側をチェック
	for (Corner corner : {kRightTop, kRightBottom}) {
		auto index = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[corner]);
		auto mapChip = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (mapChip == MapChipType::kBlock) {
			hit = true;
			break;
		}
	}

	if (hit) {
		// 現在の右端位置と、移動後の右端位置で、マスをまたいだかどうかチェック
		Vector3 playerRightPosNow = worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0);
		Vector3 playerRightPosNext = worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0);

		auto indexNow = mapChipField_->GetMapChipIndexSetByPosition(playerRightPosNow);
		auto indexNext = mapChipField_->GetMapChipIndexSetByPosition(playerRightPosNext);

		if (indexNow.xIndex != indexNext.xIndex) {
			auto rect = mapChipField_->GetRectByIndex(indexNext.xIndex, indexNext.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.isHitWall = true;
		}
	}
}

// 中身入れるのは02_08スライド25枚目
// 左当たり判定
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	// 右向き移動なら判定不要
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;

	// 左上の角をチェック
	{
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (type == MapChipType::kBlock) {
			hit = true;
		}
	}

	// 左下の角をチェック
	{
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);
		if (type == MapChipType::kBlock) {
			hit = true;
		}
	}

	if (hit) {
		// 移動後の左端の座標（プレイヤーの左端）
		Vector3 leftPosAfterMove = worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0);
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(leftPosAfterMove);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex);

		// 右端との重なりを補正：そのマスの右端までしか移動させない
		float overlap = leftPosAfterMove.x - rect.right;
		if (overlap < 0.0f) {
			info.move.x -= overlap + kBlank; // +kBlankで微小な隙間を空ける
		}
		info.isHitWall = true;
	}
}

// 接地状態の切り替え処理02_08_page_14
void Player::UpdateOnGround(const CollisionMapInfo& info) {
	// 自分キャラが接地状態？
	if (onGround_) {
		// 接地状態の処理
		// ジャンプ開始02_08_page_18
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定
			// 落下なら空中状態に切り替え
			std::array<Vector3, kNumCorner> positionNew;

			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}
			// 02_08_page_19
			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 空中状態の処理
		// 着地フラグ
		if (info.isHitLanding) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 接地時にX速度を減衰
			velocity_.x *= (1.0f, -kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

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
void Player::Update() { BehabiorRootUpdate(); }

// 描画
void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
// 02_10 10枚目
Vector3 Player::GetWorldPosition() {

	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
// 02_10 14枚目
AABB Player::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	// ジャンプ開始(仮処理)
	// velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
	//
	isDead_ = true;
}
// AL3_02_14
void Player::BehabiorRootUpdate() {

	// 移動処理が中に入っている関数AL3_02_07 p10
	Player::InputMove();

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
	// 02_08_page?//ここでupdate関数を呼ぶ確定よべよべよべよべ
	UpdateOnGround(collisionMapInfo);

	// 位置に加算（移動）
	// worldTransform_.translation_ += velocity_;

	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

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
}

// コンストラクタ
Player::Player() {}
// デストラクタ
Player::~Player() {}
