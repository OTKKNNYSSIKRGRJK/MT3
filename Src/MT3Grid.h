#pragma once

#include"MT3.h"
#include<cstdint>

namespace MT3 {
	struct Grid {
		const Mat4* VPVp_{ nullptr };

		uint32_t Colors_[2] { 0x9F3F3FFF, 0x3F3F9FFF, };

		float CellWidth_{ 1.0f };
		float CellHeight_{ 1.0f };
		int Num_CellsAlongXAxis{ 10 };
		int Num_CellsAlongZAxis{ 10 };

	public:
		void Draw();
	};
}