#pragma once
#include "KamataEngine.h"
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
    float life = 0.0f;
    WorldTransform wt;
};

std::vector<std::unique_ptr<Particle>> particles_;
	Model* model_ = nullptr; 
	std::mt19937 rng_{std::random_device{}()};
};
