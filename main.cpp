#include <Novice.h>

#include"HW_00/HW_00_02.h"

const char kWindowTitle[] = "LE2C_08_コウ_シキン_MT3_00_02";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	MT3::HW_00_02 hw_00_02{};

	char keys[256]{ 0 };
	char preKeys[256]{ 0 };

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		hw_00_02.Print();

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
