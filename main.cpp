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

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_03_01";

namespace {
	char keys[256]{ 0 };
	char preKeys[256]{ 0 };
}

namespace MT3 {
	struct SRT {
		Vec3 Scale;
		Vec3 Rotate;
		Vec3 Translate;
	};

	class HW_03_01 {
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

		SphereIndicator Shoulder_{};
		SphereIndicator Elbow_{};
		SphereIndicator Hand_{};
		SRT ShoulderSRT_{};
		SRT ElbowSRT_{};
		SRT HandSRT_{};
		Mat4 ShoulderLocal_{};
		Mat4 ElbowLocal_{};
		Mat4 HandLocal_{};
		Mat4 ShoulderWorld_{};
		Mat4 ElbowWorld_{};
		Mat4 HandWorld_{};

		int IsCollided_{ 0 };

		char Keys_[256]{};

	public:
		HW_03_01() {
			Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);

			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			ShoulderSRT_ = {
				.Scale{ 1.0f, 1.0f, 1.0f },
				.Rotate{ 0.0f, 0.0f, -6.8f },
				.Translate{ 0.2f, 1.0f, 0.0f },
			};
			ElbowSRT_ = {
				.Scale{ 1.0f, 1.0f, 1.0f },
				.Rotate{ 0.0f, 0.0f, -1.4f },
				.Translate{ 0.4f, 0.0f, 0.0f },
			};
			HandSRT_ = {
				.Scale{ 1.0f, 1.0f, 1.0f },
				.Rotate{ 0.0f, 0.0f, 0.0f },
				.Translate{ 0.3f, 0.0f, 0.0f },
			};

			Shoulder_.Radius = 0.0625f;
			Shoulder_.RGBA_ = 0xFF0000FF;
			Elbow_.Radius = 0.0625f;
			Elbow_.RGBA_ = 0x00FF00FF;
			Hand_.Radius = 0.0625f;
			Hand_.RGBA_ = 0x0000FFFF;
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
				ImGui::Text("");

				ImGui::SeparatorText("Shoulder");
				ImGui::Indent();
				ImGui::DragFloat3("Scale##Shoulder", ShoulderSRT_.Scale(), 0.01f);
				ImGui::DragFloat3("Rotate##Shoulder", ShoulderSRT_.Rotate(), 0.01f);
				ImGui::DragFloat3("Translate##Shoulder", ShoulderSRT_.Translate(), 0.01f);
				ImGui::Unindent();
				ImGui::SeparatorText("Elbow");
				ImGui::Indent();
				ImGui::DragFloat3("Scale##Elbow", ElbowSRT_.Scale(), 0.01f);
				ImGui::DragFloat3("Rotate##Elbow", ElbowSRT_.Rotate(), 0.01f);
				ImGui::DragFloat3("Translate##Elbow", ElbowSRT_.Translate(), 0.01f);
				ImGui::Unindent();
				ImGui::SeparatorText("Hand");
				ImGui::Indent();
				ImGui::DragFloat3("Scale##Hand", HandSRT_.Scale(), 0.01f);
				ImGui::DragFloat3("Rotate##Hand", HandSRT_.Rotate(), 0.01f);
				ImGui::DragFloat3("Translate##Hand", HandSRT_.Translate(), 0.01f);
				ImGui::Unindent();
			}
			ImGui::End();
			#endif

			ShoulderLocal_ = Mat4::MakeSRTMatrix(ShoulderSRT_.Scale, ShoulderSRT_.Rotate, ShoulderSRT_.Translate);
			ElbowLocal_ = Mat4::MakeSRTMatrix(ElbowSRT_.Scale, ElbowSRT_.Rotate, ElbowSRT_.Translate);
			HandLocal_ = Mat4::MakeSRTMatrix(HandSRT_.Scale, HandSRT_.Rotate, HandSRT_.Translate);

			ShoulderWorld_ = ShoulderLocal_;
			Mat4::Multiply(ElbowWorld_, ElbowLocal_, ShoulderWorld_);
			Mat4::Multiply(HandWorld_, HandLocal_, ElbowWorld_);

			Shoulder_.Center = ShoulderWorld_[3];
			Shoulder_.Update();
			Elbow_.Center = ElbowWorld_[3];
			Elbow_.Update();
			Hand_.Center = HandWorld_[3];
			Hand_.Update();

			Mat4::Multiply(VPVp_, View_, PVp_);
		}

		void Draw() {
			Grid_.Draw();

			Novice::SetBlendMode(kBlendModeNormal);

			Shoulder_.Draw(VPVp_);
			Elbow_.Draw(VPVp_);
			Hand_.Draw(VPVp_);

			static LineSegmentIndicator seg;
			seg.Origin = Shoulder_.Center;
			seg.Diff = Elbow_.Center - Shoulder_.Center;
			seg.Draw(VPVp_);
			seg.Origin = Elbow_.Center;
			seg.Diff = Hand_.Center - Elbow_.Center;
			seg.Draw(VPVp_);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	MT3::HW_03_01 hw{};

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
