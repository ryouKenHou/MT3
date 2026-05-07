#include <Novice.h>

#include "Vector3.hpp"
#include "Matrix4.hpp"

const char kWindowTitle[] = "LE2A_28_リョウ_ケン_ホウ";

static const int kColumnWidth = 80;
static const int kRowHeight = 20;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%0.2f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%0.2f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%0.2f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x , y, "%s", label);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			Novice::ScreenPrintf(x + kColumnWidth * j, y + kRowHeight * (i+1), "%0.2f", matrix.m[i][j]);
		}
	}	
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Vector3 rotate = Vector3(0.4f, 1.43f, -0.8f);
		Matrix4x4 rotX = Matrix4x4::MakeRotationXMatrix(rotate.x);
		Matrix4x4 rotY = Matrix4x4::MakeRotationYMatrix(rotate.y);
		Matrix4x4 rotZ = Matrix4x4::MakeRotationZMatrix(rotate.z);
		Matrix4x4 rot = rotX * rotY * rotZ;

		///
		/// ↑更新処理ここまで
		///
		
		MatrixScreenPrintf(0, 0, rotX, "rotX");
		MatrixScreenPrintf(0, kRowHeight* 5, rotY, "rotY");
		MatrixScreenPrintf(0, kRowHeight * 10, rotZ, "rotZ");
		MatrixScreenPrintf(0, kRowHeight * 15, rot, "rotXYZ");
		

		///
		/// ↓描画処理ここから
		///

		


		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
