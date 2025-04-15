#pragma once

#include<cmath>

#include<immintrin.h>

#include<initializer_list>
#include<memory>

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
	};
}