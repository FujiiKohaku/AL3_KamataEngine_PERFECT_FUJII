#include "InhaleEffect.h"
#include "Math.h"
#include <cmath>

void InhaleEffect::Initialize() {
	model_ = Model::CreateFromOBJ("Coin", true);

	// 余裕を持って確保（再配置対策）
	particles_.reserve(256);
}

void InhaleEffect::Update(const Vector3& center, bool active) {

	// 吸い込み中だけ生成
	if (active) {
		std::uniform_real_distribution<float> dist(-1.2f, 1.2f);


		for (int i = 0; i < 4; i++) {

			auto p = std::make_unique<Particle>();

			p->pos = center + Vector3(dist(rng_), dist(rng_), dist(rng_));
			p->vel = {0, 0, 0};
			p->life = 0.2f;

			p->wt.Initialize();
			p->wt.scale_ = {0.25f, 0.25f, 0.25f};

			particles_.push_back(std::move(p));
		}
	}

	// 更新
	for (auto it = particles_.begin(); it != particles_.end();) {

		auto& p = *(*it);

		Vector3 dir = center - p.pos;
		dir = Normalize(dir);

		// 吸い込み加速
		p.vel += dir * 0.05f;
		p.pos += p.vel;

		p.wt.translation_ = p.pos;
		WorldTransformUpdate(p.wt);

		p.life -= 1.0f / 60.0f;

		if (p.life <= 0.0f)
			it = particles_.erase(it);
		else
			++it;
	}
}

void InhaleEffect::Draw(Camera* camera) {
	for (auto& ptr : particles_) {
		model_->Draw(ptr->wt, *camera);
	}
}
