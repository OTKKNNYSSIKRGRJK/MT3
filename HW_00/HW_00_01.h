#pragma once

#include"../Src/MT3Vec3.h"

namespace MT3 {
	class HW_00_01 {
	private:
		Vec3 V1_{};
		Vec3 V2_{};
		float K_{};

		Vec3 V1AddV2_{};
		Vec3 V1SubV2_{};
		Vec3 KMulV1_{};
		float V1DotV2_{};
		float Len_V1_{};
		Vec3 Norm_V2_{};

	public:
		HW_00_01(
			const Vec3& v1_ = { 1.0f, 3.0f, -5.0f },
			const Vec3& v2_ = { 4.0f, -1.0f, 2.0f },
			float k_ = 4.0f
		) : V1_{ v1_ }, V2_{ v2_ }, K_{ k_ } {
			V1AddV2_ = V1_ + V2_;
			V1SubV2_ = V1_ - V2_;
			KMulV1_ = K_ * V1_;
			V1DotV2_ = Vec3::Dot(V1_, V2_);
			Len_V1_ = V1_.Length();
			Norm_V2_ = V2_.Norm();
		}

		~HW_00_01() = default;
		
	public:
		void Print() const;
	};
}