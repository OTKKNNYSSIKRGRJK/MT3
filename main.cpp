#include <Novice.h>

#include"Src/MT3.h"
#include"Src/MT3Grid.h"
#include"Src/Sphere.h"
#include"Src/Line.h"
#include"Src/Triangle.h"
#include"Src/AABB.h"
#include"Src/Collision.h"
#include<format>
#include<vector>
#include<algorithm>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_02_05";

namespace MT3 {
	namespace {
		#if defined(_DEBUG)
		void ImGuiPrintMat4(const Mat4& m_) {
			for (int i = 0; i < 4; ++i) {
				ImGui::Text(
					"%7.2f%7.2f%7.2f%7.2f", m_[i][0], m_[i][1], m_[i][2], m_[i][3]
				);
			};
		}
		#endif
	}

	class HW_02_05 {
	private:
		Vec3 CameraRotate_{ 0.6f, 0.6f, 0.0f };
		Vec3 CameraTranslate_{ -5.25f, 7.0f, -7.5f };

		Mat4 Camera_{};

		Mat4 View_{};
		Mat4 Projection_{ Mat4::MakePerspectiveFOV(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f) };
		Mat4 Viewport_{ Mat4::MakeViewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f) };
		Mat4 PVp_{};
		//Mat4 Inv_PVp_{};
		Mat4 VPVp_{};

		Grid Grid_{};

		AABB AABB0_{};
		AABB AABB1_{};

		int IsCollided_{ 0 };

		char Keys_[256]{};

	public:
		HW_02_05() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			AABB0_ = {
				.X_Min = 1.0f,
				.Y_Min = 1.0f,
				.Z_Min = 1.0f,
				.X_Max = 2.0f,
				.Y_Max = 2.0f,
				.Z_Max = 2.0f,
			};
			AABB1_ = {
				.X_Min = -1.0f,
				.Y_Min = -1.0f,
				.Z_Min = -1.0f,
				.X_Max = 0.0f,
				.Y_Max = 0.0f,
				.Z_Max = 0.0f,
			};
		}

		void Update() {
			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText("Camera");
				ImGui::DragFloat3("Rotate##Camera", CameraRotate_(), 0.01f);
				ImGui::DragFloat3("Translate##Camera", CameraTranslate_(), 0.01f);
			}
			ImGui::End();
			#endif

			Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);
			Mat4::Multiply(VPVp_, View_, PVp_);

			static_cast<AABBIndicator&>(AABB0_).Update("AABB0");
			static_cast<AABBIndicator&>(AABB1_).Update("AABB1");

			IsCollided_ = IsCollided(AABB0_, AABB1_);
			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::Text("%d", IsCollided_);
			}
			ImGui::End();
			#endif
		}

		void Draw() {
			Grid_.Draw();

			if (IsCollided_) {
				static_cast<const AABBIndicator&>(AABB0_).Draw(VPVp_, 0xFF3F3FDF);
				static_cast<const AABBIndicator&>(AABB1_).Draw(VPVp_, 0x3F3FFFDF);
			}
			else {
				static_cast<const AABBIndicator&>(AABB0_).Draw(VPVp_, 0xFFDFDF7F);
				static_cast<const AABBIndicator&>(AABB1_).Draw(VPVp_, 0xDFDFFF7F);
			}
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_02_05 hw{};

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		hw.Update();
		hw.Draw();

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
