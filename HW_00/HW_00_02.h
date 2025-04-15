#pragma once

#include"../Src/MT3Mat4.h"

namespace MT3 {
	class HW_00_02 {
	private:
		Mat4 M1_{
			3.2f, 0.7f, 9.6f, 4.4f,
			5.5f, 1.3f, 7.8f, 2.1f,
			6.9f, 8.0f, 2.6f, 1.0f,
			0.5f, 7.2f, 5.1f, 3.3f,
		};
		Mat4 M2_{
			4.1f, 6.5f, 3.3f, 2.2f,
			8.8f, 0.6f, 9.9f, 7.7f,
			1.1f, 5.5f, 6.6f, 0.0f,
			3.3f, 9.9f, 8.8f, 2.2f,
		};

		Mat4 CalcResults_[8];

	public:
		HW_00_02();
		~HW_00_02() = default;

	public:
		void Print() const;
	};
}