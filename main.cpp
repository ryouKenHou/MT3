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

		Vector3 translate = Vector3(4.1f, 2.6f, 0.8f);
		Vector3 scale = Vector3(1.5f, 5.2f, 7.3f);

		Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(translate);
		Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(scale);
		Vector3 point = Vector3(2.3f, 3.8f, 1.4f);
		Matrix4x4 transformMatrix = {
			1.0f, 2.0f, 3.0f, 4.0f,
			3.0f, 1.0f , 1.0f, 2.0f,
			1.0f, 4.0f , 2.0f, 3.0f,
			2.0f, 2.0f , 1.0f, 3.0f,
		};

		Vector3 transformedPoint = Matrix4x4::TransformPoint(transformMatrix, point);

		///
		/// ↑更新処理ここまで
		///
		
		VectorScreenPrintf(0, 0, transformedPoint, "transformed");
		MatrixScreenPrintf(0, 40, translateMatrix, "translateMatrix");
		MatrixScreenPrintf(0, 40 + kRowHeight * 5, scaleMatrix, "scaleMatrix");

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
