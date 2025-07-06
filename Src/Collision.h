#pragma once

#include"MT3.h"
#include"Line.h"
#include"Triangle.h"
#include"Plane.h"

#include"AABB.h"

namespace MT3 {
	bool IsCollided(const LineSegment& seg_, const Triangle& tri_) {
		Plane plane{};
		plane.Normal_ = Vec3::Cross(
			tri_.Vertices_[1] - tri_.Vertices_[0],
			tri_.Vertices_[2] - tri_.Vertices_[1]
		);
		plane.Normal_ = plane.Normal_.Norm();
		// Dot(triangle.Vertices_[0], plane.Normal_) ==
		// Dot(triangle.Vertices_[1], plane.Normal_) ==
		// Dot(triangle.Vertices_[2], plane.Normal_)
		plane.Distance_ = Vec3::Dot(tri_.Vertices_[0], plane.Normal_);

		float t = (plane.Distance_ - Vec3::Dot(plane.Normal_, seg_.Origin) / Vec3::Dot(plane.Normal_, seg_.Diff));
		Vec3 pointOnPlane = seg_.Origin + seg_.Diff * t;

		if ((t >= 0.0f) && (t <= 1.0f)) {
			Vec3&& v01{ tri_.Vertices_[1] - tri_.Vertices_[0] };
			Vec3&& v12{ tri_.Vertices_[2] - tri_.Vertices_[1] };
			Vec3&& v20{ tri_.Vertices_[0] - tri_.Vertices_[2] };

			Vec3&& v0p{ pointOnPlane - tri_.Vertices_[0] };
			Vec3&& v1p{ pointOnPlane - tri_.Vertices_[1] };
			Vec3&& v2p{ pointOnPlane - tri_.Vertices_[2] };

			Vec3&& v01CrossV1p{ Vec3::Cross(v01, v1p) };
			Vec3&& v12CrossV2p{ Vec3::Cross(v12, v2p) };
			Vec3&& v20CrossV0p{ Vec3::Cross(v20, v0p) };

			return (
				(Vec3::Dot(v01CrossV1p, plane.Normal_) >= 0.0f) &&
				(Vec3::Dot(v12CrossV2p, plane.Normal_) >= 0.0f) &&
				(Vec3::Dot(v20CrossV0p, plane.Normal_) >= 0.0f)
			);
		}

		return false;
	}

	bool IsCollided(const AABB& aabb0_, const AABB& aabb1_) {
		return (
			(aabb0_.X_Min <= aabb1_.X_Max) &&
			(aabb0_.X_Max >= aabb1_.X_Min) &&
			(aabb0_.Y_Min <= aabb1_.Y_Max) &&
			(aabb0_.Y_Max >= aabb1_.Y_Min) &&
			(aabb0_.Z_Min <= aabb1_.Z_Max) &&
			(aabb0_.Z_Max >= aabb1_.Z_Min)
		);
	}
}