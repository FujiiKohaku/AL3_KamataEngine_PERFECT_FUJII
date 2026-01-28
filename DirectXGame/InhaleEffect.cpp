#include "InhaleEffect.h"
#include "Math.h"
#include <algorithm>

void InhaleEffect::Initialize() {

	model_ = Model::CreateFromOBJ("vacuumParticle", true);

	particles_.reserve(kMaxParticles);
}

void InhaleEffect::Update(const Vector3& center, bool active) {

	frameCount_++;

	// =========================
	// 生成（吸い込み中のみ）
	// =========================
	if (active) {

		spawnTimer_ -= 1.0f / 60.0f;

		if (spawnTimer_ <= 0.0f) {
			spawnTimer_ = 0.05f;

			if (particles_.size() < kMaxParticles) {

				std::uniform_real_distribution<float> dist(-1.2f, 1.2f);

				for (int i = 0; i < 2; i++) {

					auto p = std::make_unique<Particle>();

					p->pos = center + Vector3(dist(rng_), dist(rng_), dist(rng_));
					p->vel = {0, 0, 0};
					p->life = 0.2f;
					p->alive = true;

					p->wt.Initialize();
					p->wt.scale_ = {0.1f, 0.1f, 0.1f};

					particles_.push_back(std::move(p));
				}
			}
		}
	}

	// =========================
	// 更新
	// =========================
	for (auto& ptr : particles_) {

		Particle& p = *ptr;

		if (!p.alive) {
			continue;
		}

		Vector3 dir = center - p.pos;
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);


		if (len > 0.001f) {
			p.vel += dir * (0.05f / len);
		}

		p.pos += p.vel;
		p.wt.translation_ = p.pos;

		// GPU転送は間引く
		if ((frameCount_ & 1) == 0) {
			WorldTransformUpdate(p.wt);
		}

		p.life -= 1.0f / 60.0f;

		if (p.life <= 0.0f) {
			p.alive = false;
		}
	}

	// =========================
	// 定期クリーンアップ
	// =========================
	CleanupDeadParticles();
}

void InhaleEffect::CleanupDeadParticles() {

	cleanupTimer_++;

	if (cleanupTimer_ < 30) {
		return;
	}

	cleanupTimer_ = 0;

	particles_.erase(std::remove_if(particles_.begin(), particles_.end(), [](const std::unique_ptr<Particle>& p) { return !p->alive; }), particles_.end());
}

void InhaleEffect::Draw(Camera* camera) {

	for (auto& ptr : particles_) {

		if (!ptr->alive) {
			continue;
		}

		model_->Draw(ptr->wt, *camera);
	}
}
