#include <Novice.h>

#include"Src/MT3.h"
#include<format>
#include<vector>

#include<ImGui.h>

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_01_02";

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

		inline float GetWorldPosXGivenScreenPosX(const Mat4& t_, float worldZ_, float screenX_) {
			return
				(screenX_ * (worldZ_ * t_[2][3] + t_[3][3]) - (worldZ_ * t_[2][0] + t_[3][0])) /
				(t_[0][0] - screenX_ * t_[0][3]);
		}
		inline float GetWorldPosXGivenScreenPosY(const Mat4& t_, float worldZ_, float screenY_) {
			return
				(screenY_ * (worldZ_ * t_[2][3] + t_[3][3]) - (worldZ_ * t_[2][1] + t_[3][1])) /
				(t_[0][1] - screenY_ * t_[0][3]);
		}
		inline float GetWorldPosZGivenScreenPosX(const Mat4& t_, float worldX_, float screenX_) {
			return
				(screenX_ * (worldX_ * t_[0][3] + t_[3][3]) - (worldX_ * t_[0][0] + t_[3][0])) /
				(t_[2][0] - screenX_ * t_[2][3]);
		}
		inline float GetWorldPosZGivenScreenPosY(const Mat4& t_, float worldX_, float screenY_) {
			return
				(screenY_ * (worldX_ * t_[0][3] + t_[3][3]) - (worldX_ * t_[0][1] + t_[3][1])) /
				(t_[2][1] - screenY_ * t_[2][3]);
		}

		using GetWorldPosFunc = float(*)(const Mat4&, float, float);
	}

	struct Grid {
		const Mat4* VPVp_{ nullptr };
		const Mat4* Inv_VPVp_{ nullptr };

	private:
		struct {
			UINT CornerIndexList[4];
			UINT IsScanningHorizontalEdge;
			UINT IsScanningWorldPosZ;
			UINT RGBA;
		} ScanEdgeParams_[8]{
			{ { 0, 2, 3, 1, }, 1, 0, 0x9F3F3FFF },
			{ { 1, 0, 2, 3, }, 0, 0, 0x9F3F3FFF },
			{ { 3, 1, 0, 2, }, 1, 0, 0x9F3F3FFF },
			{ { 2, 3, 1, 0, }, 0, 0, 0x9F3F3FFF },
			{ { 0, 2, 3, 1, }, 1, 1, 0x3F3F9FFF },
			{ { 1, 0, 2, 3, }, 0, 1, 0x3F3F9FFF },
			{ { 3, 1, 0, 2, }, 1, 1, 0x3F3F9FFF },
			{ { 2, 3, 1, 0, }, 0, 1, 0x3F3F9FFF },
		};

	public:
		void Draw() {
			Vec3 screenPoses[2]{};

			static Vec3 cornerScreenPoses[4]{
				{ 0.0f, 0.0f, 0.0f },
				{ 1280.0f, 0.0f, 0.0f },
				{ 0.0f, 720.0f, 0.0f },
				{ 1280.0f, 720.0f, 0.0f }
			};
			static Vec3 cornerWorldPoses[4]{};
			const float minusInv_Inv_VPVp21{ -1.0f / (*Inv_VPVp_)[2][1] };
			for (int i{ 0 }; i < 4; ++i) {
				cornerScreenPoses[i].z =
					minusInv_Inv_VPVp21 * (
						cornerScreenPoses[i].x * (*Inv_VPVp_)[0][1] +
						cornerScreenPoses[i].y * (*Inv_VPVp_)[1][1] +
						(*Inv_VPVp_)[3][1]
					);
				cornerWorldPoses[i] = cornerScreenPoses[i] * (*Inv_VPVp_);
			}

			#if defined(_DEBUG)
			ImGui::Begin("Corners");
			{
				if (ImGui::BeginTable("CornerPosTable", 3, ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersV)) {
					ImGui::TableSetupColumn("#", 0, 0.1f);
					ImGui::TableSetupColumn("Screen Pos");
					ImGui::TableSetupColumn("World Pos");
					ImGui::TableHeadersRow();
					for (int row{ 0 }; row < 4; ++row) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%d", row);
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(
							"(%5.0f, %5.0f, %10f)",
							cornerScreenPoses[row].x,
							cornerScreenPoses[row].y,
							cornerScreenPoses[row].z
						);
						ImGui::TableSetColumnIndex(2);
						ImGui::Text(
							"(%10f, %.0f, %10f)",
							cornerWorldPoses[row].x,
							cornerWorldPoses[row].y,
							cornerWorldPoses[row].z
						);
					}
					ImGui::EndTable();
				}
			}
			ImGui::End();
			#endif

			static auto ScanEdge{
				[](UINT indices_[4], UINT isScanningHorizontalEdge_, UINT isScanningWorldPosZ_, const Mat4& vpVp_, UINT rgba_) {
					static GetWorldPosFunc GetWorldPosZFuncs[4]{
						GetWorldPosZGivenScreenPosX,
						GetWorldPosZGivenScreenPosY,
						GetWorldPosXGivenScreenPosX,
						GetWorldPosXGivenScreenPosY,
					};

					Vec3 screenPoses[2]{};

					isScanningHorizontalEdge_ = !!(isScanningHorizontalEdge_);
					isScanningWorldPosZ_ = !!(isScanningWorldPosZ_);
					float scanComp{ 
						std::ceil(
							(isScanningWorldPosZ_) ?
							(cornerWorldPoses[indices_[0]].z) :
							(cornerWorldPoses[indices_[0]].x)
						)
					};
					for (int i{ 1 }; i < 4; ++i) {
						float thresholdComp{
							(isScanningWorldPosZ_) ?
							(std::min<float>(cornerWorldPoses[indices_[3]].z, cornerWorldPoses[indices_[i]].z)) :
							(std::min<float>(cornerWorldPoses[indices_[3]].x, cornerWorldPoses[indices_[i]].x))
						};
						while (scanComp <= thresholdComp) {
							const float stComp{
								GetWorldPosZFuncs[isScanningHorizontalEdge_ + 2 * isScanningWorldPosZ_](
									vpVp_, scanComp,
									(isScanningHorizontalEdge_) ?
									(cornerScreenPoses[indices_[0]].y) :
									(cornerScreenPoses[indices_[0]].x)
								)
							};
							const float edComp{
								GetWorldPosZFuncs[((isScanningHorizontalEdge_ + i) & 1) + 2 * isScanningWorldPosZ_](
									vpVp_, scanComp,
									((isScanningHorizontalEdge_ + i) & 1) ?
									(cornerScreenPoses[indices_[i]].y) :
									(cornerScreenPoses[indices_[i]].x)
								)
							};
							const Vec3 st{ (isScanningWorldPosZ_) ? (stComp) : (scanComp), 0.0f, (isScanningWorldPosZ_) ? (scanComp) : (stComp) };
							const Vec3 ed{ (isScanningWorldPosZ_) ? (edComp) : (scanComp), 0.0f, (isScanningWorldPosZ_) ? (scanComp) : (edComp) };
							screenPoses[0] = st * vpVp_;
							screenPoses[1] = ed * vpVp_;
							if (screenPoses[0].z < 0.9999f || screenPoses[1].z < 0.9999f) {
								Novice::DrawLine(
									static_cast<int>(screenPoses[0].x),
									static_cast<int>(screenPoses[0].y),
									static_cast<int>(screenPoses[1].x),
									static_cast<int>(screenPoses[1].y),
									rgba_
								);
							}
							if (scanComp == 0.0f) {
								Novice::DrawLine(
									static_cast<int>(screenPoses[0].x),
									static_cast<int>(screenPoses[0].y),
									static_cast<int>(screenPoses[1].x),
									static_cast<int>(screenPoses[1].y),
									0xFFFFFF3F
								);
							}
							scanComp += 1.0f;
						}
					}
				}
			};

			Novice::SetBlendMode(kBlendModeAdd);
			for (int i{ 0 }; i < 8; ++i) {
				ScanEdge(
					ScanEdgeParams_[i].CornerIndexList,
					ScanEdgeParams_[i].IsScanningHorizontalEdge,
					ScanEdgeParams_[i].IsScanningWorldPosZ,
					*VPVp_,
					ScanEdgeParams_[i].RGBA
				);
			}
		}
	};

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
			#if defined(_DEBUG)
			ImGui::Begin("Sphere");
			{
				ImGui::DragFloat3("Scale", Scale_(), 0.01f);
				ImGui::DragFloat3("Rotate", Rotate_(), 0.01f);
				ImGui::DragFloat3("Translate", Translate_(), 0.01f);
			}
			ImGui::End();
			#endif

			//Rotate_.x += 0.007f;
			Rotate_.y += 0.01f;
		}

		void Draw() {
			World_ = Mat4::MakeSRTMatrix(Scale_, Rotate_, Translate_);
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

	class HW_01_02 {
	private:
		Vec3 CameraScale_{ 1.0f, 1.0f, 1.0f };
		Vec3 CameraRotate_{ 0.35f, -0.1f, 0.0f };
		Vec3 CameraTranslate_{ 2.0f, 10.0f, -30.0f };
		Mat4 Camera_{};

		Mat4 View_{};
		Mat4 Projection_{ Mat4::MakePerspectiveFOV(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f) };
		Mat4 Viewport_{ Mat4::MakeViewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f) };
		Mat4 PVp_{};
		Mat4 Inv_PVp_{};
		Mat4 VPVp_{};
		Mat4 Inv_VPVp_{};

		Sphere Sphere_{ 0.25f, 6U, 12U };

		Grid Grid_{};

	public:
		HW_01_02() {
			Mat4::Multiply(PVp_, Projection_, Viewport_);
			Mat4::Invert(Inv_PVp_, PVp_);

			Sphere_.VPVp_ = &VPVp_;
			Grid_.VPVp_ = &VPVp_;
			Grid_.Inv_VPVp_ = &Inv_VPVp_;
		}

		void Update() {
			#if defined(_DEBUG)
			ImGui::Begin("Camera");
			{
				ImGui::DragFloat3("Rotate", CameraRotate_(), 0.01f);
				ImGui::DragFloat3("Translate", CameraTranslate_(), 0.01f);
			}
			ImGui::End();
			#endif

			Camera_ = Mat4::MakeSRTMatrix(
				CameraScale_,
				CameraRotate_,
				CameraTranslate_
			);
			Mat4::Invert(View_, Camera_);
			Mat4::Multiply(VPVp_, View_, PVp_);
			Mat4::Multiply(Inv_VPVp_, Inv_PVp_, Camera_);

			Sphere_.Update();
		}

		void Draw() {
			Grid_.Draw();
			Sphere_.Draw();
		}
	};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	MT3::HW_01_02 hw{};

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
