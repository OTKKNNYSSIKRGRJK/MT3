#include"HW_00_01.h"

#include <Novice.h>

namespace MT3 {
	void HW_00_01::Print() const {
		Novice::ScreenPrintf(0, 0, "v1 + v2 = (%.2f, %.2f, %.2f)", V1AddV2_.x, V1AddV2_.y, V1AddV2_.z);
		Novice::ScreenPrintf(0, 20, "v1 - v2 = (%.2f, %.2f, %.2f)", V1SubV2_.x, V1SubV2_.y, V1SubV2_.z);
		Novice::ScreenPrintf(0, 40, "k * v1 = (%.2f, %.2f, %.2f)", KMulV1_.x, KMulV1_.y, KMulV1_.z);
		Novice::ScreenPrintf(0, 60, "v1 dot v2 = %.2f", V1DotV2_);
		Novice::ScreenPrintf(0, 80, "Length of v1 = %.2f", Len_V1_);
		Novice::ScreenPrintf(0, 100, "Norm of v2 = (%.2f, %.2f, %.2f)", Norm_V2_.x, Norm_V2_.y, Norm_V2_.z);
	}
}