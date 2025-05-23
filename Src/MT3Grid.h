#pragma once

#include"MT3.h"
#include<cstdint>

namespace MT3 {
	struct Grid {
		const Mat4* VPVp_{ nullptr };
		const Mat4* Inv_VPVp_{ nullptr };

	private:
		struct {
			uint32_t CornerIndexList[4];
			uint32_t IsScanningHorizontalEdge;
			uint32_t IsScanningWorldPosZ;
			uint32_t RGBA;
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
		void Draw();
	};
}