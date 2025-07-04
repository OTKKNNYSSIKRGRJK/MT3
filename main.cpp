#include <Novice.h>

#include"Src/MT3.h"
#include"Src/MT3Grid.h"
#include"Src/Sphere.h"
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

		char Keys_[256]{};

	public:
		HW_02_05() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			Seg_.Origin = { -0.45f, 0.4f, -2.0f };
			Seg_.Diff = { 0.0f, 0.0f, 5.0f };

			Triangle_.Vertices_[0] = { -2.0f, 0.0f, -0.3f };
			Triangle_.Vertices_[1] = { -0.3f, 1.6f, 1.5f };
			Triangle_.Vertices_[2] = { 0.7f, -0.2f, 0.0f };

			PointIndicator_.Scale_ = { 0.0625f, 0.0625f, 0.0625f };
		}

		void Update() {
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText("Camera");
				ImGui::DragFloat3("Rotate##Camera", CameraRotate_(), 0.01f);
				ImGui::DragFloat3("Translate##Camera", CameraTranslate_(), 0.01f);
			}
			ImGui::End();

			Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);
			Mat4::Multiply(VPVp_, View_, PVp_);

			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			ImGui::SeparatorText("Segment");
			{
				ImGui::DragFloat3("Origin##Segment", Seg_.Origin(), 0.01f);
				ImGui::DragFloat3("Diff##Segment", Seg_.Diff(), 0.01f);
			}
			ImGui::SeparatorText("Triangle");
			{
				ImGui::DragFloat3("Vert0##Triangle", Triangle_.Vertices_[0](), 0.01f);
				ImGui::DragFloat3("Vert1##Triangle", Triangle_.Vertices_[1](), 0.01f);
				ImGui::DragFloat3("Vert2##Triangle", Triangle_.Vertices_[2](), 0.01f);
			}
			ImGui::End();
			#endif

			if (IsCollided(Seg_, Triangle_)) {
				Seg_.RGBA = 0xDF2F08DF;
				PointIndicator_.RGBA_ = 0xDF2F08DF;
			}
			else {
				Seg_.RGBA = 0xDFDFDFDF;
				PointIndicator_.RGBA_ = 0xDFDFDFDF;
			}

			PointIndicator_.Update();
		}

		void Draw() {
			Grid_.Draw();

			Seg_.Draw(VPVp_);

			Triangle_.Draw(VPVp_);
			PointIndicator_.Draw(VPVp_);
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
