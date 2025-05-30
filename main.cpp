#include <Novice.h>

#include"Src/MT3.h"
#include"Src/MT3Grid.h"
#include<format>
#include<vector>
#include<algorithm>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_02_02";

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

	Vec3 Perpendicular(const Vec3& v_) {
		if (v_.x != 0.0f || v_.y != 0.0f) { return { -v_.y, v_.x, 0.0f }; }
		return { 0.0f, -v_.z, v_.y };
	}

	struct Plane {
		Vec3 Normal_;
		float Distance_;
	};

	struct PlaneIndicator : public Plane {
	public:
		const Mat4* VPVp_{ nullptr };
		float Scale_{ 3.0f };

		void Draw() {
			Vec3&& center = Distance_ * Normal_;
			Vec3&& v0_P = Perpendicular(Normal_).Norm() * Scale_;
			Vec3&& v1_P = Vec3::Cross(Normal_, v0_P);
			Vec3 ps[4]{
				(center + v0_P) * (*VPVp_),
				(center - v1_P) * (*VPVp_),
				(center + v1_P) * (*VPVp_),
				(center - v0_P) * (*VPVp_),
			};
			Novice::DrawQuad(
				static_cast<int>(ps[0].x), static_cast<int>(ps[0].y),
				static_cast<int>(ps[1].x), static_cast<int>(ps[1].y),
				static_cast<int>(ps[2].x), static_cast<int>(ps[2].y),
				static_cast<int>(ps[3].x), static_cast<int>(ps[3].y),
				0, 0, 1, 1, 0, 0xFFFFFF0F
			);
			Novice::DrawLine(
				static_cast<int>(ps[0].x), static_cast<int>(ps[0].y),
				static_cast<int>(ps[1].x), static_cast<int>(ps[1].y),
				0xFFFFFFBF
			);
			Novice::DrawLine(
				static_cast<int>(ps[1].x), static_cast<int>(ps[1].y),
				static_cast<int>(ps[3].x), static_cast<int>(ps[3].y),
				0xFFFFFFBF
			);
			Novice::DrawLine(
				static_cast<int>(ps[3].x), static_cast<int>(ps[3].y),
				static_cast<int>(ps[2].x), static_cast<int>(ps[2].y),
				0xFFFFFFBF
			);
			Novice::DrawLine(
				static_cast<int>(ps[2].x), static_cast<int>(ps[2].y),
				static_cast<int>(ps[0].x), static_cast<int>(ps[0].y),
				0xFFFFFFBF
			);
		}
	};

	class HW_02_02 {
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

		Sphere Sphere_{};
		float SphereRadius_{};
		PlaneIndicator Plane_{};

		Grid Grid_{};

		char Keys_[256]{};

		bool IsCollided(const Sphere& s_, const Plane& pl_) {
			float dist = std::abs(Vec3::Dot(s_.Translate_, pl_.Normal_) - pl_.Distance_);
			float radius = s_.Scale_.x;
			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText("Collision");
				ImGui::Text("Distance = %f", dist);
				ImGui::Text("Radius of sphere = %f", radius);
			}
			ImGui::End();
			#endif
			return (dist <= radius);
		}

	public:
		HW_02_02() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			//Mat4::Invert(Inv_PVp_, PVp_);

			Grid_.VPVp_ = &VPVp_;

			Sphere_.Translate_ = { -0.140f, -0.840f, 0.670f };
			Sphere_.VPVp_ = &VPVp_;
			SphereRadius_ = 1.5f;

			Plane_.VPVp_ = &VPVp_;
			Plane_.Normal_ = Vec3{ -0.967f, 0.237f, -0.097f }.Norm();
			Plane_.Distance_ = 1.8f;
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
			ImGui::SeparatorText("Sphere");
			{
				ImGui::DragFloat3("Center##Sphere", Sphere_.Translate_(), 0.01f);
				ImGui::DragFloat("Radius##Sphere", &SphereRadius_, 0.01f);
			}
			ImGui::SeparatorText("Plane");
			{
				ImGui::DragFloat3("Normal##Plane", Plane_.Normal_(), 0.01f);
				ImGui::DragFloat("Distance##Plane", &Plane_.Distance_, 0.01f);
			}
			ImGui::End();
			#endif

			Sphere_.Scale_ = { SphereRadius_, SphereRadius_, SphereRadius_ };
			Sphere_.Update();

			Plane_.Normal_ = Plane_.Normal_.Norm();

			if (IsCollided(Sphere_, Plane_)) {
				Sphere_.RGBA_ = 0xDF1F2F7F;
			}
			else {
				Sphere_.RGBA_ = 0xFFFFFF7F;
			}
		}

		void Draw() {
			Grid_.Draw();

			Sphere_.Draw();
			Plane_.Draw();

			Vec3 projOnPlane =
				Sphere_.Translate_ +
				(Plane_.Distance_ - Vec3::Dot(Sphere_.Translate_, Plane_.Normal_)) * Plane_.Normal_;
			Vec3 p0 = Sphere_.Translate_ * VPVp_;
			Vec3 p1 = projOnPlane * VPVp_;
			Novice::DrawLine(
				static_cast<int>(p0.x), static_cast<int>(p0.y),
				static_cast<int>(p1.x), static_cast<int>(p1.y),
				0xFFFF7F7F
			);
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_02_02 hw{};

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
