#include "DeathParticles.h"

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// 02_11_13枚目 モデルとカメラを退避
	model_ = model;
	camera_ = camera;

	// 02_11_11枚目 ワールド変換の初期化
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	// 02_11_31枚目
	objectColor_.Initialize();

	// 02_11_31枚目
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {

	// 02_11_12枚目
	// ワールド行列更新
	for (auto& worldTransform : worldTransforms_) {
		WorldTransformUpdate(worldTransform);
	}

	// 02_11_27枚目  終了なら何もしない
	if (isFinished_) {
		return;
	}

	// 02_11_26枚目  カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 02_11_26枚目  存続時間の上限に達したら
	if (counter_ >= kDuration_) {
		counter_ = kDuration_;
		// 終了扱いにする
		isFinished_ = true;
	}

	// 02_11_page_23
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// kSpeedを使用して速度ベクトルを用意
		Vector3 velocity = {kSpeed_, 0, 0};

		// angleで8個のパーティクルに角度を与える
		float angle = kAngleUnit_ * i;

		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);

		// オリジナル数学関数ファイルにTransform関数追加
		// 方向を変えるために必要TransformはvectorとMatrixの掛け算
		velocity = Transform(velocity, matrixRotation);

		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	// 02_11_32page
	// 時間経過でアルファ値を下げて、パーティクルをフェードアウトする処理
	// clamp は「値を一定の範囲内に制限する安全装置」！
	color_.w = std::clamp(1.0f - counter_ / kDuration_, 0.0f, 1.0f);
	// 02_11_32枚目 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
}

void DeathParticles::Draw() {

	// 02_11_27枚目  終了なら何もしない
	if (isFinished_) {
		return;
	}

	// 02_11_13枚目
	for (auto& worldTransform : worldTransforms_) {
		// 02_11_33枚目で&objectColor_を追加
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}