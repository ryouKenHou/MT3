#include <Novice.h>
#include <imgui.h>

#include "Vector3.hpp"
#include "Matrix4.hpp"


const char kWindowTitle[] = "LE2A_28_リョウ_ケン_ホウ";

static const int kColumnWidth = 80;
static const int kRowHeight = 20;

// =======================================================================
// struct 
// =======================================================================

struct Sphere {
	Vector3 center;
	float radius;
};

// ==============================================================================
// 関数宣言
// =============================================================================
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%0.2f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%0.2f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%0.2f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			Novice::ScreenPrintf(x + kColumnWidth * j, y + kRowHeight * (i + 1), "%0.2f", matrix.m[i][j]);
		}
	}
}

void DrawGrid(const Matrix4x4& viewPortMatrix, const Matrix4x4& viewProjectionMatrix) {
	const float kGridHalfSize = 2.0f;
	const float kSubdivision = 10.0f;
	const float kGridEvery = (kGridHalfSize * 2) / kSubdivision;
	const int centerIndex = static_cast<int>(kSubdivision / 2);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfSize + xIndex * kGridEvery;
		Vector3 start = Transform(Vector3(x, 0, -kGridHalfSize), viewProjectionMatrix * viewPortMatrix);
		Vector3 end = Transform(Vector3(x, 0, kGridHalfSize), viewProjectionMatrix * viewPortMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), xIndex == centerIndex ? 0x000000FF : 0xFFFFFFAA);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfSize + zIndex * kGridEvery;
		Vector3 start = Transform(Vector3(-kGridHalfSize, 0, z), viewProjectionMatrix * viewPortMatrix);
		Vector3 end = Transform(Vector3(kGridHalfSize, 0, z), viewProjectionMatrix * viewPortMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), zIndex == centerIndex ? 0x000000FF : 0xFFFFFFAA);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivisoin = 20;
	const float pi = 3.14159265358979323846f;
	const float kLonEvery = (pi * 2 / kSubdivisoin);
	const float kLatEvery = (pi / kSubdivisoin);


	for (uint32_t latIndex = 0; latIndex < kSubdivisoin; ++latIndex) {
		float lat = -pi / 2.0f + latIndex * kLatEvery;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivisoin; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			Vector3 a, b, c;
			a.x = sphere.center.x + sphere.radius * cosf(lat) * cosf(lon);
			a.y = sphere.center.y + sphere.radius * sinf(lat);
			a.z = sphere.center.z + sphere.radius * cosf(lat) * sinf(lon);

			b.x = sphere.center.x + sphere.radius * cosf(lat + kLatEvery) * cosf(lon);
			b.y = sphere.center.y + sphere.radius * sinf(lat + kLatEvery);
			b.z = sphere.center.z + sphere.radius * cosf(lat + kLatEvery) * sinf(lon);

			c.x = sphere.center.x + sphere.radius * cosf(lat) * cosf(lon + kLonEvery);
			c.y = sphere.center.y + sphere.radius * sinf(lat);
			c.z = sphere.center.z + sphere.radius * cosf(lat) * sinf(lon + kLonEvery);

			Vector3 screenA = Transform(a, viewProjectionMatrix * viewportMatrix);
			Vector3 screenB = Transform(b, viewProjectionMatrix * viewportMatrix);
			Vector3 screenC = Transform(c, viewProjectionMatrix * viewportMatrix);

			Novice::DrawLine(static_cast<int>(screenA.x), static_cast<int>(screenA.y), static_cast<int>(screenB.x), static_cast<int>(screenB.y), color);
			Novice::DrawLine(static_cast<int>(screenA.x), static_cast<int>(screenA.y), static_cast<int>(screenC.x), static_cast<int>(screenC.y), color);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraPos = { 0.f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.f, 0.f };
	Sphere sphere = { Vector3(0, 0, 0), 1.0f };

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
		ImGui::Begin("Camera Info");
		ImGui::DragFloat3("Camera Position", &cameraPos.x, 0.1f);
		ImGui::DragFloat3("Camera Rotation", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere Position", &sphere.center.x, 0.1f);
		ImGui::DragFloat("Sphere Radius", &sphere.radius, 0.1f, 0.1f, 10.0f);
		ImGui::End();

		Matrix4x4 cameraMatrix = Matrix4x4::MakeAffineMatrix(Vector3(1, 1, 1), cameraRotate, cameraPos);
		Matrix4x4 viewMatrix = Matrix4x4::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewPortMatrix = Matrix4x4::MakeViewportMatrix(0, 0, 1280, 720, 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		DrawGrid(viewPortMatrix, viewMatrix * projectionMatrix);

		DrawSphere(sphere, viewMatrix * projectionMatrix, viewPortMatrix, BLACK);
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
