#include <Novice.h>

#include"Src/MT3.h"
#include"Src/MT3Grid.h"
#include<format>
#include<vector>
#include<algorithm>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_02_00";

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

	namespace {
		constexpr float Pi{ 3.14159265f };

		void CreateSphereVertices(
			std::vector<Vec3>& vertices_,
			const Vec3& center_,
			float radius_,
			uint32_t div1_, uint32_t div2_
		) {
			const float inv_Div1{ 1.0f * Pi / static_cast<float>(div1_) };
			const float inv_Div2{ 2.0f * Pi / static_cast<float>(div2_) };

			for (uint32_t i{ 0 }; i <= div1_; ++i) {
				for (uint32_t j{ 0 }; j < div2_; ++j) {
					const float theta{ i * inv_Div1 - 0.5f * Pi };
					const float cosTheta{ std::cos(theta) };
					const float sinTheta{ std::sin(theta) };
					const float phi{ j * inv_Div2 };
					const float cosPhi{ std::cos(phi) };
					const float sinPhi{ std::sin(phi) };

					auto& vert = vertices_.emplace_back();
					{
						vert.x = cosTheta * radius_ * cosPhi + center_.x;
						vert.y = sinTheta * radius_ + center_.y;
						vert.z = cosTheta * radius_ * sinPhi + center_.z;
					}
				}
			}
		}

		void CreateSphereGrid(
			std::vector<Vec3>& vertices_,
			std::vector<std::pair<uint32_t, uint32_t>>& indexPairs_,
			const Vec3& center_,
			float radius_,
			uint32_t div1_, uint32_t div2_
		) {
			CreateSphereVertices(vertices_, center_, radius_, div1_, div2_);

			for (uint32_t i{ 0 }; i < div1_; ++i) {
				for (uint32_t j{ 0 }; j < div2_; ++j) {
					const uint32_t p0 = i * div2_ + j;
					const uint32_t p1 = (i + 1) * div2_ + j;
					indexPairs_.emplace_back(p0, p1);
				}
			}
			for (uint32_t i{ 1 }; i < div1_; ++i) {
				for (uint32_t j{ 0 }; j < div2_; ++j) {
					const uint32_t p0 = i * div2_ + j;
					const uint32_t p1 = i * div2_ + (j + 1) % div2_;
					indexPairs_.emplace_back(p0, p1);
				}
			}
		}
	}

	struct Sphere {
	private:
		std::vector<Vec3> Vertices_{};
		std::vector<std::pair<uint32_t, uint32_t>> IndexPairs_{};

		Mat4 World_{};
		Mat4 WVPVp_{};

	public:
		Vec3 Scale_{ 1.0f, 1.0f, 1.0f };
		Vec3 Rotate_{};
		Vec3 Translate_{};

		const Mat4* VPVp_{ nullptr };

		uint32_t RGBA_{ 0xFFDFAF7F };

		Sphere(float radius_ = 3.0f, uint32_t div1_ = 12U, uint32_t div2_ = 24U) {
			CreateSphereGrid(Vertices_, IndexPairs_, { 0.0f, 0.0f, 0.0f }, radius_, div1_, div2_);
		}

		void Update() {
			/*#if defined(_DEBUG)
			ImGui::Begin("Sphere");
			{
				ImGui::DragFloat3("Scale", Scale_(), 0.01f);
				ImGui::DragFloat3("Rotate", Rotate_(), 0.01f);
				ImGui::DragFloat3("Translate", Translate_(), 0.01f);
			}
			ImGui::End();
			#endif*/
			World_ = Mat4::MakeSRTMatrix(Scale_, Rotate_, Translate_);
		}

		void Draw() {
			Mat4::Multiply(WVPVp_, World_, *VPVp_);

			Novice::SetBlendMode(kBlendModeNormal);

			for (const auto& indexPair : IndexPairs_) {
				const Vec3 st{ Vertices_[indexPair.first] * WVPVp_ };
				const Vec3 ed{ Vertices_[indexPair.second] * WVPVp_ };
				Novice::DrawLine(
					static_cast<int>(st.x),
					static_cast<int>(st.y),
					static_cast<int>(ed.x),
					static_cast<int>(ed.y),
					RGBA_
				);
			}
		}
	};

	struct LineSegment {
		Vec3 Origin;
		Vec3 Diff;
	};

	inline Vec3 ProjectPointOntoLine(const Vec3& p_, const LineSegment& l_) {
		return
			l_.Origin +
			(Vec3::Dot(l_.Diff, (p_ - l_.Origin)) / Vec3::Dot(l_.Diff, l_.Diff)) *
			l_.Diff;
	}
	inline Vec3 FindNearestPointOnSegment(const Vec3& p_, const LineSegment& seg_) {
		return
			seg_.Origin +
			std::clamp(
				Vec3::Dot(seg_.Diff, (p_ - seg_.Origin)) / Vec3::Dot(seg_.Diff, seg_.Diff),
				0.0f, 1.0f
			) * seg_.Diff;
	}

	class HW_02_00 {
	private:
		Vec3 CameraRotate_{ 0.1f, 0.0f, 0.0f };
		Vec3 CameraTranslate_{ 0.0f, 1.0f, -10.0f };
		//Vec3 CameraRotate_{ 0.35f, -0.1f, 0.0f };
		//Vec3 CameraTranslate_{ 1.0f, 4.0f, -10.0f };

		Mat4 Camera_{};

		Mat4 View_{};
		Mat4 Projection_{ Mat4::MakePerspectiveFOV(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f) };
		Mat4 Viewport_{ Mat4::MakeViewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f) };
		Mat4 PVp_{};
		//Mat4 Inv_PVp_{};
		Mat4 VPVp_{};

		LineSegment Seg_{ .Origin{ -2.0f, -1.0f, 0.0f }, .Diff{ 3.0f, 2.0f, 2.0f } };

		Sphere Point_{ 0.0625f, 6U, 12U };
		Sphere ProjectedPoint_{ 0.0625f, 6U, 12U };
		Sphere NearestPoint_{ 0.0625f, 6U, 12U };

		Grid Grid_{};

		char Keys_[256]{};

	public:
		HW_02_00() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Point_.VPVp_ = &VPVp_;
			ProjectedPoint_.VPVp_ = &VPVp_;
			NearestPoint_.VPVp_ = &VPVp_;
			Grid_.VPVp_ = &VPVp_;

			Point_.Translate_ = { -1.5f, 0.6f, 0.6f };
			Point_.RGBA_ = 0xFFDF7FFF;
			ProjectedPoint_.RGBA_ = 0xFFDF7F3F;
			NearestPoint_.RGBA_ = 0xFFFFFFFF;

			/*Mat4 camera{ CameraRotateMat_ };
			camera[3][0] = CameraTranslate_.x;
			camera[3][1] = CameraTranslate_.y;
			camera[3][2] = CameraTranslate_.z;
			Mat4::Invert(View_, camera);*/
		}

		void Update() {
			/*Novice::GetHitKeyStateAll(Keys_);
			Vec3 tmpT{}, tmpR{};
			if (Keys_[DIK_UP]) { tmpT.z = -0.05f; }
			if (Keys_[DIK_DOWN]) { tmpT.z = 0.05f; }
			if (Keys_[DIK_LEFT]) { tmpT.x = 0.05f; }
			if (Keys_[DIK_RIGHT]) { tmpT.x = -0.05f; }
			if (Keys_[DIK_W]) { tmpR.x = -0.005f; }
			if (Keys_[DIK_S]) { tmpR.x = 0.005f; }
			if (Keys_[DIK_A]) { tmpR.y = -0.005f; }
			if (Keys_[DIK_D]) { tmpR.y = 0.005f; }
			
			Mat4&& rotate{ Mat4::MakeRotateMatrix(tmpR) };
			Mat4 inv_Rotate{};
			Mat4::Invert(inv_Rotate, rotate);
			Mat4::Multiply(View_, View_, inv_Rotate);
			View_[3][0] += tmpT.x;
			View_[3][2] += tmpT.z;*/

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
			{
				ImGui::SeparatorText("Projection of point on segment");
				{
					ImGui::BulletText("Segment");
					ImGui::DragFloat3("Origin", Seg_.Origin(), 0.01f);
					ImGui::DragFloat3("Diff", Seg_.Diff(), 0.01f);
					ImGui::BulletText("Point");
					ImGui::DragFloat3("Pos", Point_.Translate_(), 0.01f);
					ImGui::InputFloat3("Projected Pos", ProjectedPoint_.Translate_(), "%.3f", ImGuiInputTextFlags_ReadOnly);
					ImGui::InputFloat3("Nearest Pos", NearestPoint_.Translate_(), "%.3f", ImGuiInputTextFlags_ReadOnly);
				}
			}
			ImGui::End();
			#endif

			Point_.Update();

			ProjectedPoint_.Translate_ = ProjectPointOntoLine(Point_.Translate_, Seg_);
			ProjectedPoint_.Update();
			NearestPoint_.Translate_ = FindNearestPointOnSegment(Point_.Translate_, Seg_);
			NearestPoint_.Update();
		}

		void Draw() {
			Grid_.Draw();
			Point_.Draw();

			ProjectedPoint_.Draw();
			Vec3 st{ Seg_.Origin * VPVp_ };
			Vec3 ed{ ProjectedPoint_.Translate_ * VPVp_ };
			Novice::DrawLine(
				static_cast<int>(st.x),
				static_cast<int>(st.y),
				static_cast<int>(ed.x),
				static_cast<int>(ed.y),
				0xFFDF7F3F
			);

			NearestPoint_.Draw();
			ed = (Seg_.Origin + Seg_.Diff) * VPVp_;
			Novice::DrawLine(
				static_cast<int>(st.x),
				static_cast<int>(st.y),
				static_cast<int>(ed.x),
				static_cast<int>(ed.y),
				0xFFFFFFFF
			);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_02_00 hw{};

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
