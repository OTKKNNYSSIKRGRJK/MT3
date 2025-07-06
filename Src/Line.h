#pragma once

#include"MT3.h"

#include<Novice.h>

namespace MT3 {
	struct LineSegment {
		Vec3 Origin;
		Vec3 Diff;
		uint32_t RGBA{ 0xFFFFFFFF };
	};

	struct LineSegmentIndicator : public LineSegment {
		void Draw(const Mat4& vpVp_) const {
			Vec3 p0 = Origin;
			Vec3 p1 = Origin + Diff;

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
}