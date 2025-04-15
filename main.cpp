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

	class HW_00_04 {
	private:
		Vec3 Rotate_{};
		Mat4 RotateMats_[4]{};

	public:
		HW_00_04(const Vec3& rotate_ = { 0.4f, 1.43f, -0.8f }) {
			RotateMats_[0] = Mat4::MakeRotateXMatrix(rotate_.x);
			RotateMats_[1] = Mat4::MakeRotateYMatrix(rotate_.y);
			RotateMats_[2] = Mat4::MakeRotateZMatrix(rotate_.z);
			RotateMats_[3] = RotateMats_[0];
			RotateMats_[3] *= RotateMats_[1];
			RotateMats_[3] *= RotateMats_[2];
		}

		void Print() const {
			Novice::ScreenPrintf(0, 0, "Rotate X matrix");
			PrintMat4(RotateMats_[0], 0, 20);
			Novice::ScreenPrintf(0, 120, "Rotate Y matrix");
			PrintMat4(RotateMats_[1], 0, 140);
			Novice::ScreenPrintf(0, 240, "Rotate Z matrix");
			PrintMat4(RotateMats_[2], 0, 260);
			Novice::ScreenPrintf(0, 360, "Rotate XYZ matrix");
			PrintMat4(RotateMats_[3], 0, 380);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_00_04 hw{};

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
