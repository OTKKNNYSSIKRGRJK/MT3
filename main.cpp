#include <Novice.h>

#include"Src/MT3.h"
#include<format>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_01_00";

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

	class HW_01_00 {
	private:
		Mat4 Ortho_{};
		Mat4 PerspectiveFOV_{};
		Mat4 Viewport_{};

	public:
		HW_01_00() {
			Ortho_ = Mat4::MakeOrthographic(-160.0f, 160.0f, 200.0f, 300.0f, 0.0f, 1000.0f);
			PerspectiveFOV_ = Mat4::MakePerspectiveFOV(0.63f, 1.33f, 0.1f, 1000.0f);
			Viewport_ = Mat4::MakeViewport(100.0f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);
		}

		void Print() const {
			Novice::ScreenPrintf(0, 0, "Orthographic matrix");
			PrintMat4(Ortho_, 0, 20);
			Novice::ScreenPrintf(0, 100, "Perspective FOV matrix");
			PrintMat4(PerspectiveFOV_, 0, 120);
			Novice::ScreenPrintf(0, 200, "Viewport matrix");
			PrintMat4(Viewport_, 0, 220);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_01_00 hw{};

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
