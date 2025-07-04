#pragma once

#include<vector>
#include<numbers>
#include"MT3.h"
#include<Novice.h>

namespace MT3 {
	static void CreateSphereVertices(
		std::vector<Vec3>& vertices_,
		const Vec3& center_,
		float radius_,
		uint32_t div1_, uint32_t div2_
	) {
		const float inv_Div1{ 1.0f * std::numbers::pi_v<float> / static_cast<float>(div1_) };
		const float inv_Div2{ 2.0f * std::numbers::pi_v<float> / static_cast<float>(div2_) };

		for (uint32_t i{ 0 }; i <= div1_; ++i) {
			for (uint32_t j{ 0 }; j < div2_; ++j) {
				const float theta{ i * inv_Div1 - 0.5f * std::numbers::pi_v<float> };
				const float cosTheta{ std::cos(theta) };
				const float sinTheta{ std::sin(theta) };
				const float phi{ j * inv_Div2 };
				const float cosPhi{ std::cos(phi) };
				const float sinPhi{ std::sin(phi) };

				auto& vert = vertices_.emplace_back();
				{
					vert.x = cosTheta * radius_ * cosPhi + center_.x;
					vert.y = sinTheta * radius_ + center_.y;
					vert.z = cosTheta * radius_ * sinPhi + center_.z;
				}
			}
		}
	}

	static void CreateSphereGrid(
		std::vector<Vec3>& vertices_,
		std::vector<std::pair<uint32_t, uint32_t>>& indexPairs_,
		const Vec3& center_,
		float radius_,
		uint32_t div1_, uint32_t div2_
	) {
		CreateSphereVertices(vertices_, center_, radius_, div1_, div2_);

		for (uint32_t i{ 0 }; i < div1_; ++i) {
			for (uint32_t j{ 0 }; j < div2_; ++j) {
				const uint32_t p0 = i * div2_ + j;
				const uint32_t p1 = (i + 1) * div2_ + j;
				indexPairs_.emplace_back(p0, p1);
			}
		}
		for (uint32_t i{ 1 }; i < div1_; ++i) {
			for (uint32_t j{ 0 }; j < div2_; ++j) {
				const uint32_t p0 = i * div2_ + j;
				const uint32_t p1 = i * div2_ + (j + 1) % div2_;
				indexPairs_.emplace_back(p0, p1);
			}
		}
	}

	struct Sphere {
	private:
		std::vector<Vec3> Vertices_{};
		std::vector<std::pair<uint32_t, uint32_t>> IndexPairs_{};

		Mat4 World_{};
		mutable Mat4 WVPVp_{};

	public:
		Vec3 Scale_{ 1.0f, 1.0f, 1.0f };
		Vec3 Rotate_{};
		Vec3 Translate_{};

		uint32_t RGBA_{ 0xFFFFFF7F };

		Sphere(float radius_ = 1.0f, uint32_t div1_ = 12U, uint32_t div2_ = 24U) {
			CreateSphereGrid(Vertices_, IndexPairs_, { 0.0f, 0.0f, 0.0f }, radius_, div1_, div2_);
		}

		void Update() {
			/*#if defined(_DEBUG)
			ImGui::Begin("Sphere");
			{
				ImGui::DragFloat3("Scale", Scale_(), 0.01f);
				ImGui::DragFloat3("Rotate", Rotate_(), 0.01f);
				ImGui::DragFloat3("Translate", Translate_(), 0.01f);
			}
			ImGui::End();
			#endif*/
			World_ = Mat4::MakeSRTMatrix(Scale_, Rotate_, Translate_);
		}

		void Draw(const Mat4& vpVp_) const {
			Mat4::Multiply(WVPVp_, World_, vpVp_);

			for (const auto& indexPair : IndexPairs_) {
				const Vec3 st{ Vertices_[indexPair.first] * WVPVp_ };
				const Vec3 ed{ Vertices_[indexPair.second] * WVPVp_ };
				Novice::DrawLine(
					static_cast<int>(st.x),
					static_cast<int>(st.y),
					static_cast<int>(ed.x),
					static_cast<int>(ed.y),
					RGBA_
				);
			}
		}
	};
}