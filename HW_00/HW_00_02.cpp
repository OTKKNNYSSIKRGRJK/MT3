#include"HW_00_02.h"

#include<Novice.h>

namespace MT3 {
	namespace {
		void PrintMat4(const Mat4& m_, int x_, int y_) {
			for (int i{ 0 }; i < 4; ++i) {
				Novice::ScreenPrintf(
					x_, y_ + i * 20,
					"%7.2f%7.2f%7.2f%7.2f",
					m_[i][0], m_[i][1], m_[i][2], m_[i][3]
				);
			}
		}
	}

	HW_00_02::HW_00_02() {
		Mat4::Add(CalcResults_[0], M1_, M2_);
		Mat4::Subtract(CalcResults_[1], M1_, M2_);
		Mat4::Multiply(CalcResults_[2], M1_, M2_);
		Mat4::Invert(CalcResults_[3], M1_);
		Mat4::Invert(CalcResults_[4], M2_);
		Mat4::Transpose(CalcResults_[5], M1_);
		Mat4::Transpose(CalcResults_[6], M2_);
	}

	void HW_00_02::Print() const {
		Novice::ScreenPrintf(0, 0, "m1 + m2");
		PrintMat4(CalcResults_[0], 0, 20);
		Novice::ScreenPrintf(0, 100, "m1 - m2");
		PrintMat4(CalcResults_[1], 0, 120);
		Novice::ScreenPrintf(0, 200, "m1 * m2");
		PrintMat4(CalcResults_[2], 0, 220);
		Novice::ScreenPrintf(300, 0, "Inverse of m1");
		PrintMat4(CalcResults_[3], 300, 20);
		Novice::ScreenPrintf(300, 100, "Inverse of m2");
		PrintMat4(CalcResults_[4], 300, 120);
		Novice::ScreenPrintf(600, 0, "Transpose of m1");
		PrintMat4(CalcResults_[5], 600, 20);
		Novice::ScreenPrintf(600, 100, "Transpose of m2");
		PrintMat4(CalcResults_[6], 600, 120);
		Novice::ScreenPrintf(600, 200, "Identity");
		PrintMat4(CalcResults_[7], 600, 220);
	}
}