#pragma once

#include<Novice.h>
#include<ImGui.h>
#include<format>

namespace MT3 {
	struct AABB {
		float X_Min;
		float Y_Min;
		float Z_Min;
		float X_Max;
		float Y_Max;
		float Z_Max;
	};

	struct AABBIndicator : public AABB {
		void Update(const char* name_) {
			#if defined(_DEBUG)
			ImGui::Begin("MT3");
			{
				ImGui::SeparatorText(name_);
				std::string labels[] = {
					std::format("MinX, MinY, MinZ##{}", name_),
					std::format("MaxX, MaxY, MaxZ##{}", name_),
				};
				ImGui::DragFloat3(labels[0].data(), &X_Min, 0.01f);
				ImGui::DragFloat3(labels[1].data(), &X_Max, 0.01f);
			}
			ImGui::End();
			#endif

			if (X_Min >= X_Max) { X_Min = X_Max - 0.01f; }
			if (Y_Min >= Y_Max) { Y_Min = Y_Max - 0.01f; }
			if (Z_Min >= Z_Max) { Z_Min = Z_Max - 0.01f; }
		}

		void Draw(const Mat4& vpVp_, unsigned int rgba_ = 0xFFFFFFFF) const {
			LineSegment sides[12]{
				{ .Origin = { X_Min, Y_Min, Z_Min }, .Diff = { X_Max - X_Min, 0.0f, 0.0f } },
				{ .Origin = { X_Min, Y_Min, Z_Min }, .Diff = { 0.0f, Y_Max - Y_Min, 0.0f } },
				{ .Origin = { X_Min, Y_Min, Z_Min }, .Diff = { 0.0f, 0.0f, Z_Max - Z_Min } },
				  
				{ .Origin = { X_Min, Y_Max, Z_Max }, .Diff = { X_Max - X_Min, 0.0f, 0.0f } },
				{ .Origin = { X_Min, Y_Max, Z_Max }, .Diff = { 0.0f, Y_Min - Y_Max, 0.0f } },
				{ .Origin = { X_Min, Y_Max, Z_Max }, .Diff = { 0.0f, 0.0f, Z_Min - Z_Max } },
				  
				{ .Origin = { X_Max, Y_Max, Z_Min }, .Diff = { X_Min - X_Max, 0.0f, 0.0f } },
				{ .Origin = { X_Max, Y_Max, Z_Min }, .Diff = { 0.0f, Y_Min - Y_Max, 0.0f } },
				{ .Origin = { X_Max, Y_Max, Z_Min }, .Diff = { 0.0f, 0.0f, Z_Max - Z_Min } },
				  
				{ .Origin = { X_Max, Y_Min, Z_Max }, .Diff = { X_Min - X_Max, 0.0f, 0.0f } },
				{ .Origin = { X_Max, Y_Min, Z_Max }, .Diff = { 0.0f, Y_Max - Y_Min, 0.0f } },
				{ .Origin = { X_Max, Y_Min, Z_Max }, .Diff = { 0.0f, 0.0f, Z_Min - Z_Max } },
			};

			for (int i = 0; i < 12; ++i) {
				sides[i].RGBA = rgba_;
				const LineSegmentIndicator& segDrawer{ sides[i] };
				segDrawer.Draw(vpVp_);
			}
		}
	};
}