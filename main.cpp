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

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_02_07";

namespace {
	char keys[256]{ 0 };
	char preKeys[256]{ 0 };
}

namespace MT3 {
	class HW_02_07 {
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

		AABB AABB_{};
		LineSegmentIndicator Seg_{};

		int IsCollided_{ 0 };

		char Keys_[256]{};

	public:
		HW_02_07() {
			Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);

			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			AABB_ = {
				.X_Min = 1.0f,
				.Y_Min = 1.0f,
				.Z_Min = 1.0f,
				.X_Max = 2.0f,
				.Y_Max = 2.0f,
				.Z_Max = 2.0f,
			};

			Seg_.Diff = { 1.0f, 1.0f, 0.0f };
			Seg_.RGBA = 0xFFFFFFFF;
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
				/*ImGui::SeparatorText("Camera");
				ImGui::DragFloat3("Rotate##Camera", CameraRotate_(), 0.01f);
				ImGui::DragFloat3("Translate##Camera", CameraTranslate_(), 0.01f);*/
				ImGui::SeparatorText("Line Segment");
				ImGui::DragFloat3("Origin##Seg", Seg_.Origin(), 0.01f);
				ImGui::DragFloat3("Diff##Seg", Seg_.Diff(), 0.01f);
			}
			ImGui::End();
			#endif

			/*Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);*/
			Mat4::Multiply(VPVp_, View_, PVp_);

			static_cast<AABBIndicator&>(AABB_).Update("AABB");

			/*#if defined(_DEBUG)
			Vec3&& closetPoint = ClosestPoint(AABB_, Sphere_);
			ImGui::Begin("MT3");
			{
				ImGui::Text("ClosestPoint = (%f, %f, %f)", closetPoint.x, closetPoint.y, closetPoint.z);
			}
			ImGui::End();
			#endif*/
			IsCollided_ = IsCollided(AABB_, Seg_);
		}

		void Draw() {
			Grid_.Draw();

			if (IsCollided_) {
				static_cast<const AABBIndicator&>(AABB_).Draw(VPVp_, 0xFF3F3FDF);
			}
			else {
				static_cast<const AABBIndicator&>(AABB_).Draw(VPVp_, 0xFFDFDF7F);
			}

			Seg_.Draw(VPVp_);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	MT3::HW_02_07 hw{};

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
