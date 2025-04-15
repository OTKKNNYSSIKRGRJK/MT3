#include <Novice.h>

#include"Src/MT3.h"
#include<format>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_00_05";

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

	class HW_00_05 {
	private:
		Mat4 WorldMat_{};

	public:
		HW_00_05(
			const Vec3& scale_ = { 1.2f, 0.79f, -2.1f },
			const Vec3& rotate_ = { 0.4f, 1.43f, -0.8f },
			const Vec3& translate_ = { 2.7f, -4.15f, 1.57f }
		) {
			WorldMat_ = Mat4::MakeSRTMatrix(scale_, rotate_, translate_);
		}

		void Print() const {
			Novice::ScreenPrintf(0, 0, "World matrix");
			PrintMat4(WorldMat_, 0, 20);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_00_05 hw{};

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
