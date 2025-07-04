#pragma once

#include"MT3.h"

#include<Novice.h>

namespace MT3 {
	static Vec3 Perpendicular(const Vec3& v_) {
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
}