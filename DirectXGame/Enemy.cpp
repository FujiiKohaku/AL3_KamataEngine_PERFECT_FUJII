#define _USE_MATH_DEFINES
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include <cassert>
#include <cmath>
#include <numbers>
// 02_10 スライド20枚目
void Enemy::OnCollision(const Player* player) {
	// 02_15 スライド6枚目　デスフラグを立てる

	if (behavior_ == Behavior::kDefeated) {
		return; // 既に敗北しているので何もしない
	}

	// プレイヤーが攻撃中なら敵が死ぬ
	if (player->IsAttack()) {
		// 敵のふるまいを敗北に変更
		behaviorRequest_ = Behavior::kDefeated;
		isCollisionDisabled_ = true;
		// --- エフェクト生成 ---
		Vector3 effectPos;

		effectPos.x = static_cast<float>(GetWorldPosition().x);
		effectPos.y = static_cast<float>(GetWorldPosition().y);
		effectPos.z = static_cast<float>(GetWorldPosition().z);
		gameScene_->CreateHitEffect(effectPos);
		(void)player;
	}
}

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLチェック
	assert(model);

	model_ = model;

	camera_ = camera;
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// 初期座標(右側に配置)
	worldTransform_.translation_ = position;

	// 左方向を向く(プレイヤーが右向いてるよね)
	worldTransform_.rotation_.y = float(M_PI); // 180度回転

	// 02_09
	velocity_ = {-kWalkSpeed, 0, 0};

	// 02_09
	walkTimer = 0.0f;


}
void Enemy::UpDate() {
	// 変更リクエストがあったら
	// リクエストがアンノウン以外になったら
	// 行動変更のリクエストが来ていたら、
	// 安全なタイミングで実際の行動を切り替える（状態遷移制御）
	if (behaviorRequest_ != Behavior::kUnnown) {

		// ふるまい変更
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化を実行
		switch (behavior_) {

		case Enemy::Behavior::kDefeated:
		default:
			// 何のカウンターだよ
			counter_ = 0;
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnnown;
	}

	switch (behavior_) {
	case Enemy::Behavior::kWalk:
		// 02_09 16枚目 移動
		worldTransform_.translation_ += velocity_;
		// 02_09
		walkTimer += 1.0f / 60.0f;
		// 02_09 23枚目 回転アニメーション
		worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

		// 02_09 スライド8枚目 ワールド行列更新
		WorldTransformUpdate(worldTransform_);
		break;
	case Enemy::Behavior::kDefeated:
		// 02_15 15枚目
		counter_ += 1.0f / 60.0f;

		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = EaseOut(ToRadians(kDefeatedMotionAngleStart), ToRadians(kDefeatedMotionAngleEnd), counter_ / kDefeatedTime);

		WorldTransformUpdate(worldTransform_);

		if (counter_ >= kDefeatedTime) {
			isDead_ = true;
		}

		break;
	default:
		break;
	}

	// 最後の角度（度）

	// ワールド行列更新
	WorldTransformUpdate(worldTransform_);
}
// モデル描画
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

// 02_10 スライド14枚目
AABB Enemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}
// 02_10 スライド14枚目
Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
