#include"MT3Grid.h"

#include<Novice.h>
#include<ImGui.h>

namespace MT3 {
	namespace {
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

	void Grid::Draw() {
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
			[](
				uint32_t indices_[4],
				uint32_t isScanningHorizontalEdge_,
				uint32_t isScanningWorldPosZ_,
				const Mat4& vpVp_,
				uint32_t rgba_
			) {
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
}