#include <Novice.h>

#include"Src/MT3.h"
#include"Src/MT3Grid.h"
#include<format>
#include<vector>
#include<algorithm>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_02_01";

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

		uint32_t RGBA_{ 0xFFFFFF7F };

		Sphere(float radius_ = 1.0f, uint32_t div1_ = 12U, uint32_t div2_ = 24U) {
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

	class HW_02_01 {
	private:
		Vec3 CameraRotate_{ 0.5f, -0.5f, 0.0f };
		Vec3 CameraTranslate_{ 5.0f, 6.0f, -10.0f };

		Mat4 Camera_{};

		Mat4 View_{};
		Mat4 Projection_{ Mat4::MakePerspectiveFOV(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f) };
		Mat4 Viewport_{ Mat4::MakeViewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f) };
		Mat4 PVp_{};
		//Mat4 Inv_PVp_{};
		Mat4 VPVp_{};

		std::vector<Sphere> Spheres_{};
		std::vector<float> Radii_{};

		Grid Grid_{};

		char Keys_[256]{};

		bool IsCollided(const Sphere& s0_, const Sphere& s1_) {
			auto&& p01{ s1_.Translate_ - s0_.Translate_ };
			float sq_Dist = Vec3::Dot(p01, p01);
			float sq_Sum_Raii = (s0_.Scale_.x + s1_.Scale_.x) * (s0_.Scale_.x + s1_.Scale_.x);
			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText("Collision");
				ImGui::Text("Distance^2 = %f", sq_Dist);
				ImGui::Text("(Sum of Radii)^2 = %f", sq_Sum_Raii);
			}
			ImGui::End();
			#endif
			return (sq_Dist <= sq_Sum_Raii);
		}

	public:
		HW_02_01() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			auto& sphere0 = Spheres_.emplace_back();
			sphere0.VPVp_ = &VPVp_;
			sphere0.Translate_ = { 1.0f, 0.0f, 3.0f };
			auto& sphere1 = Spheres_.emplace_back();
			sphere1.VPVp_ = &VPVp_;
			sphere1.Translate_ = { -1.0f, 0.5f, -1.0f };
			Radii_.emplace_back(1.0f);
			Radii_.emplace_back(0.5f);
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
			for (int i = 0; i < static_cast<int>(Spheres_.size()); ++i) {
				ImGui::SeparatorText(std::format("Sphere #{}", i).data());
				{
					ImGui::DragFloat3(std::format("Center##Sphere#{}", i).data(), Spheres_[i].Translate_(), 0.01f);
					ImGui::DragFloat(std::format("Radius##Sphere#{}", i).data(), &Radii_[i], 0.01f);
				}
			}
			ImGui::End();
			#endif

			for (int i = 0; i < static_cast<int>(Spheres_.size()); ++i) {
				Spheres_[i].Scale_ = { Radii_[i], Radii_[i], Radii_[i] };
				Spheres_[i].Update();
			}

			if (IsCollided(Spheres_[0], Spheres_[1])) {
				Spheres_[1].RGBA_ = 0xDF1F2F7F;
			}
			else {
				Spheres_[1].RGBA_ = 0xFFFFFF7F;
			}
		}

		void Draw() {
			Grid_.Draw();

			for (int i = 0; i < static_cast<int>(Spheres_.size()); ++i) {
				Spheres_[i].Draw();
			}
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_02_01 hw{};

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
