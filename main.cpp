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

		Matrix4x4 m1 = {
			3.2f, 0.7f, 9.6f, 4.4f,
			5.5f, 1.3f, 7.8f, 2.1f,
			6.9f, 8.0f, 2.6f, 1.0f,
			0.5f, 7.2f, 5.1f, 3.3f
		};

		Matrix4x4 m2 = {
			4.1f, 6.5f, 3.3f, 2.2f,
			8.8f, 0.6f, 9.9f, 7.7f,
			1.1f, 5.5f, 6.6f, 0.0f,
			3.3f, 9.9f, 8.8f, 2.2f
		};

		Matrix4x4 resultAdd = Matrix4x4::Add(m1, m2);
		Matrix4x4 resultSub = Matrix4x4::Subtract(m1, m2);
		Matrix4x4 resultMul = Matrix4x4::Multiply(m1, m2);
		Matrix4x4 inverseM1 = Matrix4x4::Inverse(m1);
		Matrix4x4 inverseM2 = Matrix4x4::Inverse(m2);
		Matrix4x4 transposeM1 = Matrix4x4::Transpose(m1);
		Matrix4x4 transposeM2 = Matrix4x4::Transpose(m2);
		Matrix4x4 identity = Matrix4x4::Identity();


		///
		/// ↑更新処理ここまで
		///
		
		MatrixScreenPrintf(0, 0, resultAdd, "ADD: ");
		MatrixScreenPrintf(0, kRowHeight * 5, resultSub, "Subtract: ");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, resultMul, "Multiply: ");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, inverseM1, "InverseM1: ");
		MatrixScreenPrintf(0, kRowHeight * 5 * 4, inverseM2, "InverseM2: ");

		MatrixScreenPrintf(kColumnWidth* 5, 0, transposeM1, "TransposeM1: ");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5, transposeM2, "TransposeM2: ");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2, identity, "Identity: ");

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
