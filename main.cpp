#include <Novice.h>

#include"Src/MT3.h"
#include<format>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_01_01";

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

	struct Triangle {
		Vec3 Vertices_[3]{
			{ -0.5f, -0.5f, 0.0f },
			{ 0.0f, 0.5f, 0.0f },
			{ 0.5f, -0.5f, 0.0f },
		};

		Vec3 Scale_{ 1.0f, 1.0f, 1.0f };
		Vec3 Rotate_{};
		Vec3 Translate_{};

		Mat4 World_{};
		Mat4 WVPVp_{};
		//const Mat4* Camera_{ nullptr };
		const Mat4* VPVp_{ nullptr };

		Vec3 Normal_Local_{};

		Vec3 ScreenPoses_[3]{};

		void Update() {
			#if defined(_DEBUG)
			ImGui::Begin("Triangle");
			{
				for (int i = 0; i < 3; ++i) {
					ImGui::DragFloat3(std::format("Vertices[{}]", i).data(), Vertices_[i](), 0.01f);
				}
				ImGui::DragFloat3("Scale", Scale_(), 0.01f);
				ImGui::DragFloat3("Rotate", Rotate_(), 0.01f);
				ImGui::DragFloat3("Translate", Translate_(), 0.01f);
			}
			ImGui::End();
			#endif

			//Rotate_.x += 0.007f;
			Rotate_.y += 0.01f;

			const Vec3 edge01{
				Vertices_[1].x - Vertices_[0].x,
				Vertices_[1].y - Vertices_[0].y,
				Vertices_[1].z - Vertices_[0].z
			};
			const Vec3 edge12{
				Vertices_[2].x - Vertices_[1].x,
				Vertices_[2].y - Vertices_[1].y,
				Vertices_[2].z - Vertices_[1].z
			};
			Normal_Local_ = Vec3::Cross(edge01, edge12);
			Normal_Local_ = Normal_Local_.Norm();
		}

		void Draw() {
			World_ = Mat4::MakeSRTMatrix(Scale_, Rotate_, Translate_);
			Mat4::Multiply(WVPVp_, World_, *VPVp_);
			for (int i = 0; i < 3; ++i) {
				ScreenPoses_[i] = Vertices_[i] * WVPVp_;
			}

			#if defined(_DEBUG)
			ImGui::Begin("Triangle");
			{
				for (int i = 0; i < 3; ++i) {
					ImGui::Text(
						"ScreenPoses[%d] = (%f, %f, %f)",
						i,
						ScreenPoses_[i].x,
						ScreenPoses_[i].y,
						ScreenPoses_[i].z
					);
				};
			}
			ImGui::End();
			#endif

			const Vec3 edge01{
				ScreenPoses_[1].x - ScreenPoses_[0].x,
				ScreenPoses_[1].y - ScreenPoses_[0].y,
				0.0f
			};
			const Vec3 edge12{
				ScreenPoses_[2].x - ScreenPoses_[1].x,
				ScreenPoses_[2].y - ScreenPoses_[1].y,
				0.0f
			};
			Vec3 screenNorm{ Vec3::Cross(edge01, edge12) };

			/*#if defined(_DEBUG)
			ImGui::Begin("Debug");
			{
				ImGui::Text("edge01 = (%f, %f)", edge01.x, edge01.y);
				ImGui::Text("edge12 = (%f, %f)", edge12.x, edge12.y);
				ImGui::Text("screenNorm = (%f, %f, %f)", screenNorm.x, screenNorm.y, screenNorm.z);
				ImGui::Text("Normal_Local = (%f, %f, %f)", Normal_Local_.x, Normal_Local_.y, Normal_Local_.z);
			}
			ImGui::End();
			#endif*/

			uint32_t color = 0xFFFFFFFF;
			if (screenNorm.z < 0.0f) { color = 0xFFFFFF3F; }
			Novice::DrawTriangle(
				static_cast<int>(ScreenPoses_[0].x),
				static_cast<int>(ScreenPoses_[0].y),
				static_cast<int>(ScreenPoses_[1].x),
				static_cast<int>(ScreenPoses_[1].y),
				static_cast<int>(ScreenPoses_[2].x),
				static_cast<int>(ScreenPoses_[2].y),
				color, kFillModeSolid
			);

			Vec3 origin{ (Vertices_[0] + Vertices_[1] + Vertices_[2]) / 3.0f };
			Vec3 n{ (origin + Normal_Local_ * 0.5f) * WVPVp_ };
			origin = origin * WVPVp_;
			Novice::DrawLine(
				static_cast<int>(origin.x),
				static_cast<int>(origin.y),
				static_cast<int>(n.x),
				static_cast<int>(n.y),
				0x0000FFFF
			);
		}
	};

	class HW_01_01 {
	private:
		Vec3 CameraScale_{ 1.0f, 1.0f, 1.0f };
		Vec3 CameraRotate_{ 0.2f, 0.0f, 0.0f };
		Vec3 CameraTranslate_{ 0.0f, 1.0f, -5.0f };
		Mat4 Camera_{};

		Mat4 View_{};
		Mat4 Projection_{ Mat4::MakePerspectiveFOV(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f) };
		Mat4 Viewport_{ Mat4::MakeViewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f) };
		Mat4 PVp_{};
		Mat4 Inv_PVp_{};
		Mat4 VPVp_{};
		//Mat4 Inv_VPVp_{};

		Triangle Triangle_{};

	public:
		HW_01_01() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			Mat4::Invert(Inv_PVp_, PVp_);

			Triangle_.VPVp_ = &VPVp_;
		}

		void Update(const char keys[]) {
			#if defined(_DEBUG)
			ImGui::Begin("Camera");
			{
				ImGui::DragFloat3("Rotate", CameraRotate_(), 0.01f);
				ImGui::DragFloat3("Translate", CameraTranslate_(), 0.01f);
			}
			ImGui::End();
			#endif

			if (keys[DIK_W]) { Triangle_.Translate_.y += 0.02f; }
			if (keys[DIK_S]) { Triangle_.Translate_.y -= 0.02f; }
			if (keys[DIK_A]) { Triangle_.Translate_.x -= 0.02f; }
			if (keys[DIK_D]) { Triangle_.Translate_.x += 0.02f; }

			Camera_ = Mat4::MakeSRTMatrix(
				CameraScale_,
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);
			Mat4::Multiply(VPVp_, View_, PVp_);
			//Mat4::Multiply(Inv_VPVp_, Inv_PVp_, Camera_);

			Triangle_.Update();
		}

		void Draw() {
			Triangle_.Draw();
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_01_01 hw{};

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		hw.Update(keys);
		hw.Draw();

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}