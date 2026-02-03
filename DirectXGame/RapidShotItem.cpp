#include "RapidShotItem.h"
#include "Math.h"
#include "Player.h"
#include <numbers>
void RapidShotItem::Initialize(Model* model, const Vector3& pos) {

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = pos;

}

void RapidShotItem::Update() {

	if (collected_) {
		return;
	}

	worldTransform_.rotation_.y += 0.01f;

	WorldTransformUpdate(worldTransform_);
}

void RapidShotItem::Draw(Camera* camera) {

	if (collected_) {
		return;
	}

	model_->Draw(worldTransform_, *camera);
}

void RapidShotItem::OnCollision(Player* player) {

	if (collected_) {
		return;
	}

	collected_ = true;

	//効果適用
	player->ActivateRapidShot(9999.0f);
}
