#pragma once
#include "KamataEngine.h"
#include <memory>
#include <random>
#include <vector>

using namespace KamataEngine;

class InhaleEffect {
public:
	void Initialize();
	void Update(const Vector3& center, bool active);
	void Draw(Camera* camera);

private:
	struct Particle {
		Vector3 pos;
		Vector3 vel;
		float life;
		bool alive;
		WorldTransform wt;
	};

	void CleanupDeadParticles();

private:
	Model* model_ = nullptr;

	std::vector<std::unique_ptr<Particle>> particles_;

	std::mt19937 rng_{std::random_device{}()};

	float spawnTimer_ = 0.0f;
	int cleanupTimer_ = 0;
	int frameCount_ = 0;

	static constexpr int kMaxParticles = 128;
};
