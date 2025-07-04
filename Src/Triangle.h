#pragma once

#include"MT3.h"

#include<Novice.h>

namespace MT3 {
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
}