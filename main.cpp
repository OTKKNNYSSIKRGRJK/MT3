#include <Novice.h>

#include"Src/MT3.h"
#include"Src/MT3Grid.h"
#include"Src/Sphere.h"
#include<format>
#include<vector>
#include<algorithm>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_02_04";

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

		Vec3 Perpendicular(const Vec3& v_) {
			if (v_.x != 0.0f || v_.y != 0.0f) { return { -v_.y, v_.x, 0.0f }; }
			return { 0.0f, -v_.z, v_.y };
		}
	}

	struct LineSegment {
		Vec3 Origin;
		Vec3 Diff;
		uint32_t RGBA;
	};

	struct LineSegmentIndicator : public LineSegment {
		void Draw(const Mat4& vpVp_) const {
			Vec3 p0 = Origin;
			Vec3 p1 = (Origin + Diff);

			Vec3 screenP0 = p0 * vpVp_;
			Vec3 screenP1 = p1 * vpVp_;

			Novice::DrawQuad(
				static_cast<int>(screenP0.x) - 1, static_cast<int>(screenP0.y),
				static_cast<int>(screenP1.x) - 1, static_cast<int>(screenP1.y),
				static_cast<int>(screenP0.x) + 1, static_cast<int>(screenP0.y),
				static_cast<int>(screenP1.x) + 1, static_cast<int>(screenP1.y),
				0, 0, 1, 1, 0, RGBA
			);
			Novice::DrawQuad(
				static_cast<int>(screenP0.x), static_cast<int>(screenP0.y) - 1,
				static_cast<int>(screenP1.x), static_cast<int>(screenP1.y) - 1,
				static_cast<int>(screenP0.x), static_cast<int>(screenP0.y) + 1,
				static_cast<int>(screenP1.x), static_cast<int>(screenP1.y) + 1,
				0, 0, 1, 1, 0, RGBA
			);
		}
	};

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

	struct Triangle {
		Vec3 Vertices_[3];
	};

	struct TriangleIndicator : public Triangle {
		void Draw(const Mat4& vpVp_) {
			Vec3 screenVerts[3]{
				Vertices_[0] * vpVp_,
				Vertices_[1] * vpVp_,
				Vertices_[2] * vpVp_,
			};

			Novice::DrawLine(
				static_cast<int>(screenVerts[0].x), static_cast<int>(screenVerts[0].y),
				static_cast<int>(screenVerts[1].x), static_cast<int>(screenVerts[1].y),
				0xFFFFFFBF
			);
			Novice::DrawLine(
				static_cast<int>(screenVerts[1].x), static_cast<int>(screenVerts[1].y),
				static_cast<int>(screenVerts[2].x), static_cast<int>(screenVerts[2].y),
				0xFFFFFFBF
			);
			Novice::DrawLine(
				static_cast<int>(screenVerts[2].x), static_cast<int>(screenVerts[2].y),
				static_cast<int>(screenVerts[0].x), static_cast<int>(screenVerts[0].y),
				0xFFFFFFBF
			);
			Novice::DrawTriangle(
				static_cast<int>(screenVerts[0].x), static_cast<int>(screenVerts[0].y),
				static_cast<int>(screenVerts[1].x), static_cast<int>(screenVerts[1].y),
				static_cast<int>(screenVerts[2].x), static_cast<int>(screenVerts[2].y),
				0xFFFFFF0F,
				kFillModeSolid
			);
		}
	};

	class HW_02_04 {
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

		LineSegmentIndicator Seg_{};
		TriangleIndicator Triangle_{};
		Sphere PointIndicator_{};

		Grid Grid_{};

		char Keys_[256]{};

		bool IsCollided(const LineSegment& seg_, const Triangle& tri_) {
			Plane plane{};
			plane.Normal_ = Vec3::Cross(
				Triangle_.Vertices_[1] - Triangle_.Vertices_[0],
				Triangle_.Vertices_[2] - Triangle_.Vertices_[1]
			);
			plane.Normal_ = plane.Normal_.Norm();
			// Dot(triangle.Vertices_[0], plane.Normal_) ==
			// Dot(triangle.Vertices_[1], plane.Normal_) ==
			// Dot(triangle.Vertices_[2], plane.Normal_)
			plane.Distance_ = Vec3::Dot(Triangle_.Vertices_[0], plane.Normal_);

			float t = -(Vec3::Dot(plane.Normal_, seg_.Origin) / Vec3::Dot(plane.Normal_, seg_.Diff));
			Vec3 pointOnPlane = seg_.Origin + seg_.Diff * t;
			PointIndicator_.Translate_ = pointOnPlane;
			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText("Collision");
				ImGui::Text("t = %f", t);
			}
			ImGui::End();
			#endif

			if ((t >= 0.0f) && (t <= 1.0f)) {
				Vec3&& v01{ tri_.Vertices_[1] - tri_.Vertices_[0] };
				Vec3&& v12{ tri_.Vertices_[2] - tri_.Vertices_[1] };
				Vec3&& v20{ tri_.Vertices_[0] - tri_.Vertices_[2] };
				
				Vec3&& v0p{ pointOnPlane - tri_.Vertices_[0] };
				Vec3&& v1p{ pointOnPlane - tri_.Vertices_[1] };
				Vec3&& v2p{ pointOnPlane - tri_.Vertices_[2] };
				
				Vec3&& v01CrossV1p{ Vec3::Cross(v01, v1p) };
				Vec3&& v12CrossV2p{ Vec3::Cross(v12, v2p) };
				Vec3&& v20CrossV0p{ Vec3::Cross(v20, v0p) };
				
				return (
					(Vec3::Dot(v01CrossV1p, plane.Normal_) >= 0.0f) &&
					(Vec3::Dot(v12CrossV2p, plane.Normal_) >= 0.0f) &&
					(Vec3::Dot(v20CrossV0p, plane.Normal_) >= 0.0f)
				);
			}

			return false;
		}

		/*bool IsCollided(const LineSegment& sg_, const Triangle& tri_) {
		}*/

	public:
		HW_02_04() {
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

	MT3::HW_02_04 hw{};

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
