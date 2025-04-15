#pragma once

#include<cmath>

#include<immintrin.h>

#include<initializer_list>
#include<memory>

#pragma warning(disable : 4201)

namespace MT3 {
	class Vec3 {
	public:
		union {
			__m128 XMM_;
			float Entries_[4];
			struct { float x; float y; float z; };
		};

	public:
		inline Vec3() : XMM_{ _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f) } {}
		inline Vec3(__m128 xmm_) : XMM_{ xmm_ } { Entries_[3] = 1.0f; }
		inline Vec3(float x_, float y_, float z_) : XMM_{ _mm_setr_ps(x_, y_, z_, 1.0f) } {}
		inline Vec3(const float entries_[3]) : Vec3(entries_[0], entries_[1], entries_[2]) {}

		[[nodiscard]] inline float& operator[](int i_)& { return Entries_[i_]; }
		[[nodiscard]] inline float* operator()()& { return Entries_; }

		[[nodiscard]] inline float operator[](int i_) const& { return Entries_[i_]; }
		[[nodiscard]] inline const float* operator()() const& { return Entries_; }

		friend Vec3 operator+(const Vec3& v0_, const Vec3& v1_);
		friend Vec3 operator-(const Vec3& v0_, const Vec3& v1_);

		friend Vec3 operator*(float s_, const Vec3& v_);
		friend Vec3 operator*(const Vec3& v_, float s_);
		friend Vec3 operator/(const Vec3& v_, float s_);

		inline Vec3& operator+=(const Vec3& rhs_) {
			XMM_ = _mm_add_ps(XMM_, rhs_.XMM_);
			return *this;
		}
		inline Vec3& operator-=(const Vec3& rhs_) {
			XMM_ = _mm_sub_ps(XMM_, rhs_.XMM_);
			return *this;
		}
		inline Vec3& operator*=(float s_) {
			XMM_ = _mm_mul_ps(XMM_, _mm_set1_ps(s_));
			return *this;
		}
		inline Vec3& operator/=(float s_) {
			XMM_ = _mm_div_ps(XMM_, _mm_set1_ps(s_));
			return *this;
		}

		inline float Length() const {
			return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(XMM_, XMM_, 0x71)));
		}
		inline Vec3 Norm() const {
			return Vec3{ _mm_div_ps(XMM_, _mm_sqrt_ps(_mm_dp_ps(XMM_, XMM_, 0x77))) };
		}

		static inline float Dot(const Vec3& v0_, const Vec3& v1_) {
			return _mm_cvtss_f32(_mm_dp_ps(v0_.XMM_, v1_.XMM_, 0x71));
		}
		static inline Vec3 Cross(const Vec3& v0_, const Vec3& v1_) {
			return _mm_sub_ps(
				_mm_mul_ps(
					_mm_shuffle_ps(v0_.XMM_, v0_.XMM_, _MM_SHUFFLE(3, 0, 2, 1)),
					_mm_shuffle_ps(v1_.XMM_, v1_.XMM_, _MM_SHUFFLE(3, 1, 0, 2))
				),
				_mm_mul_ps(
					_mm_shuffle_ps(v0_.XMM_, v0_.XMM_, _MM_SHUFFLE(3, 1, 0, 2)),
					_mm_shuffle_ps(v1_.XMM_, v1_.XMM_, _MM_SHUFFLE(3, 0, 2, 1))
				)
			);
		}
	};

	inline Vec3 operator+(const Vec3& v0_, const Vec3& v1_) { return Vec3{ _mm_add_ps(v0_.XMM_, v1_.XMM_) }; }
	inline Vec3 operator-(const Vec3& v0_, const Vec3& v1_) { return Vec3{ _mm_sub_ps(v0_.XMM_, v1_.XMM_) }; }

	inline Vec3 operator*(float s_, const Vec3& v_) { return Vec3{ _mm_mul_ps(v_.XMM_, _mm_set1_ps(s_)) }; }
	inline Vec3 operator*(const Vec3& v_, float s_) { return Vec3{ _mm_mul_ps(v_.XMM_, _mm_set1_ps(s_)) }; }
	inline Vec3 operator/(const Vec3& v_, float s_) { return Vec3{ _mm_div_ps(v_.XMM_, _mm_set1_ps(s_)) }; }
}