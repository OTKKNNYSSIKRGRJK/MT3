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

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_03_02";

namespace {
	char keys[256]{ 0 };
	char preKeys[256]{ 0 };
}

namespace MT3 {
	class HW_03_02 {
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

		Vec3 VecA_{ 0.2f, 1.0f, 0.0f };
		Vec3 VecB_{ 2.4f, 3.1f, 1.2f };
		float Scalar_{ 2.4f };
		Vec3 Rotate_{ 0.4f, 1.43f, -0.8f };
		Mat4 RotMat_{};
		Mat4 RotMatX_{};
		Mat4 RotMatY_{};
		Mat4 RotMatZ_{};

		int IsCollided_{ 0 };

		char Keys_[256]{};

	public:
		HW_03_02() {
			Camera_ = Mat4::MakeSRTMatrix(
				{ 1.0f, 1.0f, 1.0f },
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);

			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;
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

				ImGui::SeparatorText("Operator Overloading");
				ImGui::BulletText("Vector");
				ImGui::Indent();
				{
					ImGui::BulletText("Inputs");
					ImGui::Indent();
					{
						ImGui::DragFloat3("A", VecA_(), 0.01f);
						ImGui::DragFloat3("B", VecB_(), 0.01f);
						ImGui::DragFloat("t", &Scalar_, 0.01f);
					}
					ImGui::Unindent();

					static Vec3 vecAPlusB{}, vecAMinusB{}, vecAMulScalar{};
					vecAPlusB = VecA_ + VecB_;
					vecAMinusB = VecA_ - VecB_;
					vecAMulScalar = VecA_ * Scalar_;

					ImGui::BulletText("Outputs");
					ImGui::Indent();
					{
						ImGui::InputFloat3("A + B", vecAPlusB(), "%.3f", ImGuiInputTextFlags_ReadOnly);
						ImGui::InputFloat3("A - B", vecAMinusB(), "%.3f", ImGuiInputTextFlags_ReadOnly);
						ImGui::InputFloat3("tA", vecAMulScalar(), "%.3f", ImGuiInputTextFlags_ReadOnly);
					}
					ImGui::Unindent();
				}
				ImGui::Unindent();

				ImGui::Text("");
				ImGui::Separator();
				ImGui::Text("");

				ImGui::BulletText("Matrix");
				ImGui::Indent();
				{
					ImGui::DragFloat3("Rotate", Rotate_(), 0.01f);

					RotMatX_ = Mat4::MakeRotateXMatrix(Rotate_.x);
					RotMatY_ = Mat4::MakeRotateYMatrix(Rotate_.y);
					RotMatZ_ = Mat4::MakeRotateZMatrix(Rotate_.z);
					RotMat_ = RotMatX_ * RotMatY_ * RotMatZ_;

					ImGui::Text("%10f %10f %10f %10f", RotMat_[0][0], RotMat_[0][1], RotMat_[0][2], RotMat_[0][3]);
					ImGui::Text("%10f %10f %10f %10f", RotMat_[1][0], RotMat_[1][1], RotMat_[1][2], RotMat_[1][3]);
					ImGui::Text("%10f %10f %10f %10f", RotMat_[2][0], RotMat_[2][1], RotMat_[2][2], RotMat_[2][3]);
					ImGui::Text("%10f %10f %10f %10f", RotMat_[3][0], RotMat_[3][1], RotMat_[3][2], RotMat_[3][3]);
				}
				ImGui::Unindent();
			}
			ImGui::End();
			#endif

			Mat4::Multiply(VPVp_, View_, PVp_);
		}

		void Draw() {
			Grid_.Draw();
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	MT3::HW_03_02 hw{};

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
