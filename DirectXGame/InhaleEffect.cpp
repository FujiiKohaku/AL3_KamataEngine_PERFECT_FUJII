#include "InhaleEffect.h"
#include "Math.h"
#include <cmath>

void InhaleEffect::Initialize() {
	model_ = Model::CreateFromOBJ("vacuumParticle", true);

	// 余裕を持って確保（再配置対策）
	particles_.reserve(256);
}

void InhaleEffect::Update(const Vector3& center, bool active) {

	// =========================
	// 吸い込み中だけ生成（間引き）
	// =========================
	if (active) {

		spawnTimer_ -= 1.0f / 60.0f;

		if (spawnTimer_ <= 0.0f) {
			spawnTimer_ = 0.05f; // ← ここが効く（1秒20回）

			if (particles_.size() < kMaxParticles) {

				std::uniform_real_distribution<float> dist(-1.2f, 1.2f);

				for (int i = 0; i < 2; i++) { // ← 4→2で十分
					auto p = std::make_unique<Particle>();

					p->pos = center + Vector3(dist(rng_), dist(rng_), dist(rng_));
					p->vel = {0, 0, 0};
					p->life = 0.2f;

					p->wt.Initialize();
					p->wt.scale_ = {0.1f, 0.1f, 0.1f};

					particles_.push_back(std::move(p));
				}
			}
		}
	}

	// =========================
	// 更新（ここはほぼ同じ）
	// =========================
	for (auto it = particles_.begin(); it != particles_.end();) {

		auto& p = *(*it);

		Vector3 dir = center - p.pos;
		dir = Normalize(dir);

		p.vel += dir * 0.05f;
		p.pos += p.vel;

		p.wt.translation_ = p.pos;
		WorldTransformUpdate(p.wt);

		p.life -= 1.0f / 60.0f;

		if (p.life <= 0.0f) {
			it = particles_.erase(it); // ← 今回は残す
		} else {
			++it;
		}
	}
}

void InhaleEffect::Draw(Camera* camera) {
	for (auto& ptr : particles_) {
		model_->Draw(ptr->wt, *camera);
	}
}
