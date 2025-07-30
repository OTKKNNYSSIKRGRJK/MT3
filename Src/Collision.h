#pragma once

#include"MT3.h"
#include"Line.h"
#include"Triangle.h"
#include"Plane.h"

#include"AABB.h"

#include<algorithm>

//#include<imgui.h>

namespace MT3 {
	inline bool IsCollided(const LineSegment& seg_, const Triangle& tri_) {
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

	inline bool IsCollided(const AABB& aabb0_, const AABB& aabb1_) {
		return (
			(aabb0_.X_Min <= aabb1_.X_Max) &&
			(aabb0_.X_Max >= aabb1_.X_Min) &&
			(aabb0_.Y_Min <= aabb1_.Y_Max) &&
			(aabb0_.Y_Max >= aabb1_.Y_Min) &&
			(aabb0_.Z_Min <= aabb1_.Z_Max) &&
			(aabb0_.Z_Max >= aabb1_.Z_Min)
		);
	}

	inline Vec3 ClosestPoint(const AABB& aabb_, const Sphere& sph_) {
		return {
			std::clamp(sph_.Center.x, aabb_.X_Min, aabb_.X_Max),
			std::clamp(sph_.Center.y, aabb_.Y_Min, aabb_.Y_Max),
			std::clamp(sph_.Center.z, aabb_.Z_Min, aabb_.Z_Max),
		};
	}

	inline bool IsCollided(const AABB& aabb_, const Sphere& sph_) {
		Vec3&& closestPoint{ ClosestPoint(aabb_, sph_) };
		Vec3&& closestPointToCenter{ sph_.Center - closestPoint };
		return sph_.Radius * sph_.Radius >= Vec3::Dot(closestPointToCenter, closestPointToCenter);
	}

	inline bool IsCollided(const AABB& aabb_, const LineSegment& seg_) {
		float txmin = (aabb_.X_Min - seg_.Origin.x) / seg_.Diff.x;
		float txmax = (aabb_.X_Max - seg_.Origin.x) / seg_.Diff.x;
		float txnear = std::min<float>(txmin, txmax);
		float txfar = std::max<float>(txmin, txmax);
		float tymin = (aabb_.Y_Min - seg_.Origin.y) / seg_.Diff.y;
		float tymax = (aabb_.Y_Max - seg_.Origin.y) / seg_.Diff.y;
		float tynear = std::min<float>(tymin, tymax);
		float tyfar = std::max<float>(tymin, tymax);
		float tzmin = (aabb_.Z_Min - seg_.Origin.z) / seg_.Diff.z;
		float tzmax = (aabb_.Z_Max - seg_.Origin.z) / seg_.Diff.z;
		float tznear = std::min<float>(tzmin, tzmax);
		float tzfar = std::max<float>(tzmin, tzmax);

		float tmin = std::max<float>(std::max<float>(txnear, tynear), tznear);
		float tmax = std::min<float>(std::min<float>(txfar, tyfar), tzfar);

		/*ImGui::Begin("Debug");
		ImGui::Text("txmin  = %f", txmin);
		ImGui::Text("txmax  = %f", txmax);
		ImGui::Text("txnear = %f", txnear);
		ImGui::Text("txfar  = %f", txfar);
		ImGui::Text("tymin  = %f", tymin);
		ImGui::Text("tymax  = %f", tymax);
		ImGui::Text("tynear = %f", tynear);
		ImGui::Text("tyfar  = %f", tyfar);
		ImGui::Text("tzmin  = %f", tzmin);
		ImGui::Text("tzmax  = %f", tzmax);
		ImGui::Text("tznear = %f", tznear);
		ImGui::Text("tzfar  = %f", tzfar);
		ImGui::Text("tmin   = %f", tmin);
		ImGui::Text("tmax   = %f", tmax);
		ImGui::Text("tmin < tmax ? %d", tmin < tmax);
		ImGui::End();*/

		if (tmin < tmax) { return (tmin <= 1.0f && tmax >= 0.0f); }
		return false;
	}
}