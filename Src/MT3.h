#pragma once

#include<cmath>

#include<immintrin.h>

#include<initializer_list>
#include<memory>

#pragma warning(disable : 4201)

namespace MT3 {
	class Vec3;
	class Mat4;
}

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

inline void Swap(__m128& xmm1_, __m128& xmm2_) {
	__m128 tmp{ xmm2_ };
	xmm2_ = xmm1_;
	xmm1_ = tmp;
}

namespace MT3 {
	class Mat4 {
	protected:
		union {
			__m128 XMMs_[4];
			__m256 YMMs_[2];
			float Entries_[4][4];
		};

	public:
		inline Mat4() {
			YMMs_[0] = _mm256_setr_ps(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			YMMs_[1] = _mm256_setr_ps(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		}

		inline Mat4(std::initializer_list<float> _initList) {
			std::memcpy(Entries_, _initList.begin(), std::min<size_t>(_initList.size(), 16Ui64) * sizeof(float));
		}

		inline float* operator[](int i_)& { return Entries_[i_]; }
		inline const float* operator[](int i_) const& { return Entries_[i_]; }

		inline Mat4& operator+=(const Mat4& rhs_) {
			Add(*this, *this, rhs_);
			return *this;
		}
		inline Mat4& operator-=(const Mat4& rhs_) {
			Subtract(*this, *this, rhs_);
			return *this;
		}
		inline Mat4& operator*=(const Mat4& rhs_) {
			Multiply(*this, *this, rhs_);
			return *this;
		}
		
		friend Vec3 operator*(const Vec3& v_, const Mat4& m_);

		// Implemented using row reduction
		float Det() const {
			Mat4 m{ *this };

			float k{ 1.0f };

			int i{ 0 };
			while (i < 4 && m.Entries_[i][0] == 0) {
				k *= -1.0f;
				++i;
			}
			if (i > 3) { return 0.0f; }
			else if (i > 0) { Swap(m.XMMs_[i], m.XMMs_[0]); }
			for (i = 1; i < 4; ++i) {
				m.XMMs_[i] = _mm_sub_ps(m.XMMs_[i], _mm_mul_ps(m.XMMs_[0], _mm_set1_ps(m.Entries_[i][0] / m.Entries_[0][0])));
			}

			i = 1;
			while (i < 4 && m.Entries_[i][1] == 0) {
				k *= -1.0f;
				++i;
			}
			if (i > 3) { return 0.0f; }
			else if (i > 1) { Swap(m.XMMs_[i], m.XMMs_[1]); }
			for (i = 2; i < 4; ++i) {
				m.XMMs_[i] = _mm_sub_ps(m.XMMs_[i], _mm_mul_ps(m.XMMs_[1], _mm_set1_ps(m.Entries_[i][1] / m.Entries_[1][1])));
			}

			i = 2;
			while (i < 4 && m.Entries_[i][2] == 0) {
				k *= -1.0f;
				++i;
			}
			if (i > 3) { return 0.0f; }
			else if (i > 2) { Swap(m.XMMs_[i], m.XMMs_[2]); }
			m.XMMs_[3] = _mm_sub_ps(m.XMMs_[3], _mm_mul_ps(m.XMMs_[2], _mm_set1_ps(m.Entries_[3][2] / m.Entries_[2][2])));

			return k * m.Entries_[0][0] * m.Entries_[1][1] * m.Entries_[2][2] * m.Entries_[3][3];
		}

	public:
		// dest_ = A_ + B_
		static inline void Add(Mat4& dest_, const Mat4& A_, const Mat4& B_) {
			dest_.YMMs_[0] = _mm256_add_ps(A_.YMMs_[0], B_.YMMs_[0]);
			dest_.YMMs_[1] = _mm256_add_ps(A_.YMMs_[1], B_.YMMs_[1]);
		}

		// dest_ = A_ - B_
		static inline void Subtract(Mat4& dest_, const Mat4& A_, const Mat4& B_) {
			dest_.YMMs_[0] = _mm256_sub_ps(A_.YMMs_[0], B_.YMMs_[0]);
			dest_.YMMs_[1] = _mm256_sub_ps(A_.YMMs_[1], B_.YMMs_[1]);
		}

		// Reference: https://stackoverflow.com/questions/18499971/efficient-4x4-matrix-multiplication-c-vs-assembly
		// dest_ = A_ * B_
		static inline void Multiply(Mat4& dest_, const Mat4& A_, const Mat4& B_) {
			__m256 a01{ _mm256_load_ps(A_.Entries_[0]) };
			__m256 a23{ _mm256_load_ps(A_.Entries_[2]) };
			__m128 b0{ _mm256_castps256_ps128(B_.YMMs_[0]) };
			__m128 b1{ _mm256_extractf128_ps(B_.YMMs_[0], 1) };
			__m128 b2{ _mm256_castps256_ps128(B_.YMMs_[1]) };
			__m128 b3{ _mm256_extractf128_ps(B_.YMMs_[1], 1) };

			__m256 dest01{ _mm256_mul_ps(_mm256_shuffle_ps(a01, a01, 0x00), _mm256_broadcast_ps(&b0)) };
			dest01 = _mm256_add_ps(dest01, _mm256_mul_ps(_mm256_shuffle_ps(a01, a01, 0x55), _mm256_broadcast_ps(&b1)));
			dest01 = _mm256_add_ps(dest01, _mm256_mul_ps(_mm256_shuffle_ps(a01, a01, 0xAA), _mm256_broadcast_ps(&b2)));
			dest01 = _mm256_add_ps(dest01, _mm256_mul_ps(_mm256_shuffle_ps(a01, a01, 0xFF), _mm256_broadcast_ps(&b3)));
			_mm256_store_ps(&dest_.Entries_[0][0], dest01);

			__m256 dest23{ _mm256_mul_ps(_mm256_shuffle_ps(a23, a23, 0x00), _mm256_broadcast_ps(&b0)) };
			dest23 = _mm256_add_ps(dest23, _mm256_mul_ps(_mm256_shuffle_ps(a23, a23, 0x55), _mm256_broadcast_ps(&b1)));
			dest23 = _mm256_add_ps(dest23, _mm256_mul_ps(_mm256_shuffle_ps(a23, a23, 0xAA), _mm256_broadcast_ps(&b2)));
			dest23 = _mm256_add_ps(dest23, _mm256_mul_ps(_mm256_shuffle_ps(a23, a23, 0xFF), _mm256_broadcast_ps(&b3)));
			_mm256_store_ps(&dest_.Entries_[2][0], dest23);
		}

		static inline void Invert(Mat4& dest_, const Mat4& src_) {
			float inv_Det{ 1.0f / src_.Det() };

			dest_[0][0] =
				inv_Det * (
					src_[1][1] * src_[2][2] * src_[3][3] +
					src_[1][2] * src_[2][3] * src_[3][1] +
					src_[1][3] * src_[2][1] * src_[3][2] -
					src_[1][3] * src_[2][2] * src_[3][1] -
					src_[1][2] * src_[2][1] * src_[3][3] -
					src_[1][1] * src_[2][3] * src_[3][2]
				);
			dest_[0][1] =
				inv_Det * (
					src_[0][3] * src_[2][2] * src_[3][1] +
					src_[0][2] * src_[2][1] * src_[3][3] +
					src_[0][1] * src_[2][3] * src_[3][2] -
					src_[0][1] * src_[2][2] * src_[3][3] -
					src_[0][2] * src_[2][3] * src_[3][1] -
					src_[0][3] * src_[2][1] * src_[3][2]
				);
			dest_[0][2] =
				inv_Det * (
					src_[0][1] * src_[1][2] * src_[3][3] +
					src_[0][2] * src_[1][3] * src_[3][1] +
					src_[0][3] * src_[1][1] * src_[3][2] -
					src_[0][3] * src_[1][2] * src_[3][1] -
					src_[0][2] * src_[1][1] * src_[3][3] -
					src_[0][1] * src_[1][3] * src_[3][2]
				);
			dest_[0][3] =
				inv_Det * (
					src_[0][3] * src_[1][2] * src_[2][1] +
					src_[0][2] * src_[1][1] * src_[2][3] +
					src_[0][1] * src_[1][3] * src_[2][2] -
					src_[0][1] * src_[1][2] * src_[2][3] -
					src_[0][2] * src_[1][3] * src_[2][1] -
					src_[0][3] * src_[1][1] * src_[2][2]
				);

			dest_[1][0] =
				inv_Det * (
					src_[1][3] * src_[2][2] * src_[3][0] +
					src_[1][2] * src_[2][0] * src_[3][3] +
					src_[1][0] * src_[2][3] * src_[3][2] -
					src_[1][0] * src_[2][2] * src_[3][3] -
					src_[1][2] * src_[2][3] * src_[3][0] -
					src_[1][3] * src_[2][0] * src_[3][2]
				);
			dest_[1][1] =
				inv_Det * (
					src_[0][0] * src_[2][2] * src_[3][3] +
					src_[0][2] * src_[2][3] * src_[3][0] +
					src_[0][3] * src_[2][0] * src_[3][2] -
					src_[0][3] * src_[2][2] * src_[3][0] -
					src_[0][2] * src_[2][0] * src_[3][3] -
					src_[0][0] * src_[2][3] * src_[3][2]
				);
			dest_[1][2] =
				inv_Det * (
					src_[0][3] * src_[1][2] * src_[3][0] +
					src_[0][2] * src_[1][0] * src_[3][3] +
					src_[0][0] * src_[1][3] * src_[3][2] -
					src_[0][0] * src_[1][2] * src_[3][3] -
					src_[0][2] * src_[1][3] * src_[3][0] -
					src_[0][3] * src_[1][0] * src_[3][2]
				);
			dest_[1][3] =
				inv_Det * (
					src_[0][0] * src_[1][2] * src_[2][3] +
					src_[0][2] * src_[1][3] * src_[2][0] +
					src_[0][3] * src_[1][0] * src_[2][2] -
					src_[0][3] * src_[1][2] * src_[2][0] -
					src_[0][2] * src_[1][0] * src_[2][3] -
					src_[0][0] * src_[1][3] * src_[2][2]
				);

			dest_[2][0] =
				inv_Det * (
					src_[1][0] * src_[2][1] * src_[3][3] +
					src_[1][1] * src_[2][3] * src_[3][0] +
					src_[1][3] * src_[2][0] * src_[3][1] -
					src_[1][3] * src_[2][1] * src_[3][0] -
					src_[1][1] * src_[2][0] * src_[3][3] -
					src_[1][0] * src_[2][3] * src_[3][1]
				);
			dest_[2][1] =
				inv_Det * (
					src_[0][3] * src_[2][1] * src_[3][0] +
					src_[0][1] * src_[2][0] * src_[3][3] +
					src_[0][0] * src_[2][3] * src_[3][1] -
					src_[0][0] * src_[2][1] * src_[3][3] -
					src_[0][1] * src_[2][3] * src_[3][0] -
					src_[0][3] * src_[2][0] * src_[3][1]
				);
			dest_[2][2] =
				inv_Det * (
					src_[0][0] * src_[1][1] * src_[3][3] +
					src_[0][1] * src_[1][3] * src_[3][0] +
					src_[0][3] * src_[1][0] * src_[3][1] -
					src_[0][3] * src_[1][1] * src_[3][0] -
					src_[0][1] * src_[1][0] * src_[3][3] -
					src_[0][0] * src_[1][3] * src_[3][1]
				);
			dest_[2][3] =
				inv_Det * (
					src_[0][3] * src_[1][1] * src_[2][0] +
					src_[0][1] * src_[1][0] * src_[2][3] +
					src_[0][0] * src_[1][3] * src_[2][1] -
					src_[0][0] * src_[1][1] * src_[2][3] -
					src_[0][1] * src_[1][3] * src_[2][0] -
					src_[0][3] * src_[1][0] * src_[2][1]
				);

			dest_[3][0] =
				inv_Det * (
					src_[1][2] * src_[2][1] * src_[3][0] +
					src_[1][1] * src_[2][0] * src_[3][2] +
					src_[1][0] * src_[2][2] * src_[3][1] -
					src_[1][0] * src_[2][1] * src_[3][2] -
					src_[1][1] * src_[2][2] * src_[3][0] -
					src_[1][2] * src_[2][0] * src_[3][1]
				);
			dest_[3][1] =
				inv_Det * (
					src_[0][0] * src_[2][1] * src_[3][2] +
					src_[0][1] * src_[2][2] * src_[3][0] +
					src_[0][2] * src_[2][0] * src_[3][1] -
					src_[0][2] * src_[2][1] * src_[3][0] -
					src_[0][1] * src_[2][0] * src_[3][2] -
					src_[0][0] * src_[2][2] * src_[3][1]
				);
			dest_[3][2] =
				inv_Det * (
					src_[0][2] * src_[1][1] * src_[3][0] +
					src_[0][1] * src_[1][0] * src_[3][2] +
					src_[0][0] * src_[1][2] * src_[3][1] -
					src_[0][0] * src_[1][1] * src_[3][2] -
					src_[0][1] * src_[1][2] * src_[3][0] -
					src_[0][2] * src_[1][0] * src_[3][1]
				);
			dest_[3][3] =
				inv_Det * (
					src_[0][0] * src_[1][1] * src_[2][2] +
					src_[0][1] * src_[1][2] * src_[2][0] +
					src_[0][2] * src_[1][0] * src_[2][1] -
					src_[0][2] * src_[1][1] * src_[2][0] -
					src_[0][1] * src_[1][0] * src_[2][2] -
					src_[0][0] * src_[1][2] * src_[2][1]
				);
		}

		static inline void Transpose(Mat4& dest_, const Mat4& src_) {
			for (int i{ 0 }; i < 4; ++i) {
				for (int j{ 0 }; j < 4; ++j) {
					dest_[i][j] = src_[j][i];
				}
			}
		}

	public:
		static inline Mat4 MakeTranslateMatrix(const Vec3& t_) {
			Mat4 tMat{};
			tMat.XMMs_[3] = t_.XMM_;
			return tMat;
		}

		static inline Mat4 MakeScaleMatrix(const Vec3& s_) {
			return {
				s_.x, 0.0f, 0.0f, 0.0f,
				0.0f, s_.y, 0.0f, 0.0f,
				0.0f, 0.0f, s_.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			};
		}

		static inline Mat4 MakeRotateXMatrix(float rx_) {
			return {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, std::cos(rx_), std::sin(rx_), 0.0f,
				0.0f, -std::sin(rx_), std::cos(rx_), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			};
		}
		static inline Mat4 MakeRotateYMatrix(float ry_) {
			return {
				std::cos(ry_), 0.0f, -std::sin(ry_), 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				std::sin(ry_), 0.0f, std::cos(ry_), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			};
		}
		static inline Mat4 MakeRotateZMatrix(float rz_) {
			return {
				std::cos(rz_), std::sin(rz_), 0.0f, 0.0f,
				-std::sin(rz_), std::cos(rz_), 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			};
		}

		static inline Mat4 MakeSRTMatrix(
			const Vec3& scale_,
			const Vec3& rotate_,
			const Vec3& translate_
		) {
			float
				cosAlpha{ std::cos(rotate_.Entries_[0]) },
				sinAlpha{ std::sin(rotate_.Entries_[0]) },
				cosBeta{ std::cos(rotate_.Entries_[1]) },
				sinBeta{ std::sin(rotate_.Entries_[1]) },
				cosGamma{ std::cos(rotate_.Entries_[2]) },
				sinGamma{ std::sin(rotate_.Entries_[2]) };

			Mat4 srt{
				cosBeta * cosGamma,
				cosBeta * sinGamma,
				-sinBeta,
				0.0f,
				sinAlpha * sinBeta * cosGamma - cosAlpha * sinGamma,
				sinAlpha * sinBeta * sinGamma + cosAlpha * cosGamma,
				sinAlpha * cosBeta,
				0.0f,
				cosAlpha * sinBeta * cosGamma + sinAlpha * sinGamma,
				cosAlpha * sinBeta * sinGamma - sinAlpha * cosGamma,
				cosAlpha * cosBeta,
				0.0f,
				translate_.Entries_[0],
				translate_.Entries_[1],
				translate_.Entries_[2],
				1.0f,
			};

			srt.XMMs_[0] = _mm_mul_ps(srt.XMMs_[0], _mm_set1_ps(scale_.Entries_[0]));
			srt.XMMs_[1] = _mm_mul_ps(srt.XMMs_[1], _mm_set1_ps(scale_.Entries_[1]));
			srt.XMMs_[2] = _mm_mul_ps(srt.XMMs_[2], _mm_set1_ps(scale_.Entries_[2]));

			return srt;
		}

	public:
		static inline Mat4 MakePerspectiveFOV(float fovY_, float aspectRatio_, float nearClip_, float farClip_) {
			const float cotTheta{ 1.0f / std::tanf(fovY_ * 0.5f) };
			const float inv_FrustumHeight{ 1.0f / (farClip_ - nearClip_) };
			return Mat4{
				(1.0f / aspectRatio_) * cotTheta, 0.0f, 0.0f, 0.0f,
				0.0f, cotTheta, 0.0f, 0.0f,
				0.0f, 0.0f, farClip_ * inv_FrustumHeight, 1.0f,
				0.0f, 0.0f, -nearClip_ * farClip_ * inv_FrustumHeight, 0.0f,
			};
		}

		static inline Mat4 MakeOrthographic(
			float l_, float t_, float r_, float b_,
			float zn_, float zf_
		) {
			const float inv_W{ 1.0f / (r_ - l_) };
			const float inv_H{ 1.0f / (t_ - b_) };
			const float inv_D{ 1.0f / (zf_ - zn_) };

			return Mat4{
				2.0f * inv_W, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f * inv_H, 0.0f, 0.0f,
				0.0f, 0.0f, inv_D, 0.0f,
				-(l_ + r_) * inv_W, -(t_ + b_) * inv_H, -zn_ * inv_D, 1.0f,
			};
		}

		static inline Mat4 MakeViewport(
			float left_, float top_,
			float width_, float height_,
			float minDepth_, float maxDepth_
		) {
			return Mat4{
				width_ * 0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -height_ * 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, maxDepth_ - minDepth_, 0.0f,
				left_ + width_ * 0.5f, top_ + height_ * 0.5f, minDepth_, 1.0f,
			};
		}
	};

	inline Vec3 operator*(const Vec3& v_, const Mat4& m_) {
		// v01 = { v_[0], v_[0], v_[0], v_[0], v_[1], v_[1], v_[1], v_[1] }
		__m256 v01{
			_mm256_set_m128(
				_mm_shuffle_ps(v_.XMM_, v_.XMM_, 0b01010101),
				_mm_shuffle_ps(v_.XMM_, v_.XMM_, 0b00000000)
			)
		};
		// v23 = { v_[2], v_[2], v_[2], v_[2], v_[3], v_[3], v_[3], v_[3] }
		__m256 v23{
			_mm256_set_m128(
				_mm_shuffle_ps(v_.XMM_, v_.XMM_, 0b11111111),
				_mm_shuffle_ps(v_.XMM_, v_.XMM_, 0b10101010)
			)
		};
		// _mm256_mul_ps(v01, m_.YMMs_[0]) =
		// { v_[0] * m_[0][0], v_[0] * m_[0][1], v_[0] * m_[0][2], v_[0] * m_[0][3],
		//   v_[1] * m_[1][0], v_[1] * m_[1][1], v_[1] * m_[1][2], v_[1] * m_[1][3] }
		// _mm256_mul_ps(v23, m_.YMMs_[1]) =
		// { v_[2] * m_[2][0], v_[2] * m_[2][1], v_[2] * m_[2][2], v_[2] * m_[2][3],
		//   v_[3] * m_[3][0], v_[3] * m_[3][1], v_[3] * m_[3][2], v_[3] * m_[3][3] }
		__m256 result256{ _mm256_add_ps(_mm256_mul_ps(v01, m_.YMMs_[0]), _mm256_mul_ps(v23, m_.YMMs_[1])) };
		__m128 result128{ _mm_add_ps(_mm256_castps256_ps128(result256), _mm256_extractf128_ps(result256, 1)) };
		float n[4]{};
		_mm_store_ps(n, result128);
		n[3] = 1.0f / n[3];
		return Vec3{ n[0] * n[3], n[1] * n[3], n[2] * n[3] };
	}
}