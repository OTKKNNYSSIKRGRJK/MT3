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

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_03_00";

namespace {
	char keys[256]{ 0 };
	char preKeys[256]{ 0 };
}

namespace MT3 {
	inline Vec3 Lerp(const Vec3& v0_, const Vec3& v1_, float t_) {
		return v0_ * (1.0f - t_) + v1_ * t_;
	}

	class HW_03_00 {
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

		std::vector<Vec3> BezierPoints_{};
		Vec3 ControlPoint0_{ -2.5f, 0.0f, 0.0f };
		Vec3 ControlPoint1_{ 0.0f, 2.5f, 0.0f };
		Vec3 ControlPoint2_{ 2.5f, 0.0f, 0.0f };

		int IsCollided_{ 0 };

		char Keys_[256]{};

		void GenerateBezier() {
			static float inv_256 = 1.0f / 256.0f;
			BezierPoints_.clear();
			BezierPoints_.emplace_back(ControlPoint0_);
			for (int i = 1; i < 256; ++i) {
				float t = i * inv_256;
				BezierPoints_.emplace_back(
					Lerp(
						Lerp(ControlPoint0_, ControlPoint1_, t),
						Lerp(ControlPoint1_, ControlPoint2_, t),
						t
					)
				);
			}
			BezierPoints_.emplace_back(ControlPoint2_);
		}

	public:
		HW_03_00() {
			Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);

			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			BezierPoints_.reserve(512);
		}

		void Update() {
			if (keys[DIK_W]) { View_[3][1] += 0.0075f * View_[3][2]; }
			if (keys[DIK_S]) { View_[3][1] -= 0.0075f * View_[3][2]; }
			if (keys[DIK_A]) { View_[3][0] -= 0.0075f * View_[3][2]; }
			if (keys[DIK_D]) { View_[3][0] += 0.0075f * View_[3][2]; }
			if (keys[DIK_Q]) { Mat4::Multiply(View_, Mat4::MakeRotateYMatrix(-0.01f), View_); }
			if (keys[DIK_E]) { Mat4::Multiply(View_, Mat4::MakeRotateYMatrix(0.01f), View_); }
			/*static int mousePrevX{ 0 }, mousePrevY{ 0 };
			static int mouseCurX{ 0 }, mouseCurY{ 0 };
			static int mouseDeltaX{}, mouseDeltaY{};
			if (Novice::IsPressMouse(0)) {
				if (Novice::IsTriggerMouse(0)) {
					Novice::GetMousePosition(&mousePrevX, &mousePrevY);
				}
				else {
					mousePrevX = mouseCurX;
					mousePrevY = mouseCurY;
				}
				Novice::GetMousePosition(&mouseCurX, &mouseCurY);
			}
			else {
				mousePrevX = mouseCurX;
				mousePrevY = mouseCurY;
			}
			mouseDeltaX = mouseCurX - mousePrevX;
			mouseDeltaY = mouseCurY - mousePrevY;
			View_[3][0] += mouseDeltaX * 0.005f;
			View_[3][1] += mouseDeltaY * (-0.005f);*/
			View_[3][2] += Novice::GetWheel() * (-0.005f);

			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText("View Operation");
				//ImGui::Text("Drag the mouse while holding the left button\nto move the view.");
				ImGui::Text("Use WASD to move the view.");
				ImGui::Text("Use Q/E to rotate the view.");
				ImGui::Text("Scroll the mouse wheel to zoom in/out.");

				ImGui::SeparatorText("Bezier");
				ImGui::DragFloat3("Control Point 0", ControlPoint0_(), 0.01f);
				ImGui::DragFloat3("Control Point 1", ControlPoint1_(), 0.01f);
				ImGui::DragFloat3("Control Point 2", ControlPoint2_(), 0.01f);
			}
			ImGui::End();
			#endif

			Mat4::Multiply(VPVp_, View_, PVp_);

			GenerateBezier();
		}

		void Draw() {
			Grid_.Draw();

			static LineSegmentIndicator seg{};
			static SphereIndicator p{};
			seg.RGBA = 0xBFBFFFFF;
			for (size_t i = 0LLU; i < BezierPoints_.size() - 1LLU; ++i) {
				seg.Origin = BezierPoints_[i];
				seg.Diff = BezierPoints_[i + 1LLU] - seg.Origin;
				seg.Draw(VPVp_);
			}
			p.RGBA_ = 0xFF0000FF;
			p.Radius = 0.0625f;
			p.Center = ControlPoint0_;
			p.Update();
			p.Draw(VPVp_);
			p.Center = ControlPoint1_;
			p.Update();
			p.Draw(VPVp_);
			p.Center = ControlPoint2_;
			p.Update();
			p.Draw(VPVp_);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	MT3::HW_03_00 hw{};

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
