#include <Novice.h>

#include"Src/MT3.h"
#include<format>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_00_03";

namespace MT3 {
	namespace {
		void PrintMat4(const Mat4& m_, int x_, int y_) {
			for (int i{ 0 }; i < 4; ++i) {
				Novice::ScreenPrintf(
					x_, y_ + i * 20,
					"%7.2f%7.2f%7.2f%7.2f",
					m_[i][0], m_[i][1], m_[i][2], m_[i][3]
				);
			}
		}
	}

	class HW_00_03 {
	private:
		Mat4 TranslateMat_{};
		Mat4 ScaleMat_{};
		Mat4 Transform_{
			1.0f, 2.0f, 3.0f, 4.0f,
			3.0f, 1.0f, 1.0f, 2.0f,
			1.0f, 4.0f, 2.0f, 3.0f,
			2.0f, 2.0f, 1.0f, 3.0f,
		};
		Vec3 TransformedPoint_{};

	public:
		HW_00_03(
			const Vec3& translate_ = { 4.1f, 2.6f, 0.8f },
			const Vec3& scale_ = { 1.5f, 5.2f, 7.3f },
			const Vec3& point_ = { 2.3f, 3.8f, 1.4f }
		) {
			TranslateMat_ = Mat4::MakeTranslateMatrix(translate_);
			ScaleMat_ = Mat4::MakeScaleMatrix(scale_);
			TransformedPoint_ = point_ * Transform_;
		}

		void Print() const {
			Novice::ScreenPrintf(
				0, 0, "Transformed point = (%.2f, %.2f, %.2f)",
				TransformedPoint_.x, TransformedPoint_.y, TransformedPoint_.z
			);

			Novice::ScreenPrintf(0, 40, "Translate matrix");
			PrintMat4(TranslateMat_, 0, 60);
			Novice::ScreenPrintf(0, 160, "Scale matrix");
			PrintMat4(ScaleMat_, 0, 180);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_00_03 hw{};

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		hw.Print();

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
