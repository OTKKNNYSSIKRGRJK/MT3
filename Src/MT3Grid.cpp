#include"MT3Grid.h"

#include<Novice.h>
#include<ImGui.h>

namespace MT3 {
	void Grid::Draw() {
		Novice::SetBlendMode(kBlendModeAdd);
		
		Vec3 st{}, ed{};

		for (int i = -(Num_CellsAlongXAxis >> 1); i <= (Num_CellsAlongXAxis >> 1); ++i) {
			st = Vec3{
				CellWidth_ * i,
				0.0f,
				CellHeight_ * (-(Num_CellsAlongZAxis >> 1))
			} * (*VPVp_);
			ed = Vec3{
				CellWidth_ * i,
				0.0f,
				CellHeight_ * (Num_CellsAlongZAxis >> 1)
			} * (*VPVp_);
			Novice::DrawLine(
				static_cast<int>(st.x),
				static_cast<int>(st.y),
				static_cast<int>(ed.x),
				static_cast<int>(ed.y),
				0x3F3F9FFF
			);
		}

		for (int i = -(Num_CellsAlongZAxis >> 1); i <= (Num_CellsAlongZAxis >> 1); ++i) {
			st = Vec3{
				CellWidth_* (-(Num_CellsAlongXAxis >> 1)),
				0.0f,
				CellHeight_* i
			} * (*VPVp_);
			ed = Vec3{
				CellWidth_* (Num_CellsAlongXAxis >> 1),
				0.0f,
				CellHeight_* i
			} * (*VPVp_);
			Novice::DrawLine(
				static_cast<int>(st.x),
				static_cast<int>(st.y),
				static_cast<int>(ed.x),
				static_cast<int>(ed.y),
				0x9F3F3FFF
			);
		}

		st = Vec3{ CellWidth_ * (-(Num_CellsAlongXAxis >> 1)), 0.0f, 0.0f } * (*VPVp_);
		ed = Vec3{ CellWidth_ * (Num_CellsAlongXAxis >> 1), 0.0f, 0.0f } * (*VPVp_);
		Novice::DrawLine(
			static_cast<int>(st.x),
			static_cast<int>(st.y),
			static_cast<int>(ed.x),
			static_cast<int>(ed.y),
			0xFFDF7F3F
		);
		st = Vec3{ 0.0f, 0.0f, CellHeight_ * (-(Num_CellsAlongZAxis >> 1)) } * (*VPVp_);
		ed = Vec3{ 0.0f, 0.0f, CellHeight_ * (Num_CellsAlongZAxis >> 1) } * (*VPVp_);
		Novice::DrawLine(
			static_cast<int>(st.x),
			static_cast<int>(st.y),
			static_cast<int>(ed.x),
			static_cast<int>(ed.y),
			0xFFDF7F3F
		);
	}
}