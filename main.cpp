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

struct Vector2i {
	int x;
	int y;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Plane{
	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 vertices[3];
};

struct AABB {
	Vector3 min;
	Vector3 max;
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
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), xIndex == centerIndex ? 0x00000055 : 0xFFFFFF55);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfSize + zIndex * kGridEvery;
		Vector3 start = Transform(Vector3(-kGridHalfSize, 0, z), viewProjectionMatrix * viewPortMatrix);
		Vector3 end = Transform(Vector3(kGridHalfSize, 0, z), viewProjectionMatrix * viewPortMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), zIndex == centerIndex ? 0x00000055 : 0xFFFFFF55);
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

Vector3 Perpendicular(const Vector3& v1) {
	if (v1.x != 0.f || v1.y != 0.f) {
		return  Vector3(-v1.y, v1.x, 0.f);
	}
	return Vector3(0.f, -v1.z, v1.y);
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = plane.normal * plane.distance;
	Vector3 perpendicular[4];
	perpendicular[0] = Perpendicular(plane.normal).Normalized();
	perpendicular[1] = {-perpendicular[0].x, -perpendicular[0].y, -perpendicular[0].z};
	perpendicular[2] = Vector3::Cross(plane.normal, perpendicular[0]);
	perpendicular[3] = { -perpendicular[2].x, -perpendicular[2].y, -perpendicular[2].z };
	Vector3 vertices[4];
	for (int i = 0; i < 4; ++i) {
		Vector3 extend = perpendicular[i] * 2.f;
		Vector3 point = center + extend;
		vertices[i] = Transform(point, viewProjectionMatrix * viewportMatrix);
	}
	Novice::DrawLine(
		static_cast<int>(vertices[3].x), static_cast<int>(vertices[3].y),
		static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y), color
	);
	Novice::DrawLine(
		static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y),
		static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y), color
	);
	Novice::DrawLine(
		static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y),
		static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y), color
	);
	Novice::DrawLine(
		static_cast<int>(vertices[3].x), static_cast<int>(vertices[3].y),
		static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y), color
	);

}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 screenVertices[3];
	for (int i = 0; i < 3; ++i) {
		screenVertices[i] = Transform(triangle.vertices[i], viewProjectionMatrix * viewportMatrix);
	}
	Novice::DrawTriangle(
		static_cast<int>(screenVertices[0].x), static_cast<int>(screenVertices[0].y),
		static_cast<int>(screenVertices[1].x), static_cast<int>(screenVertices[1].y),
		static_cast<int>(screenVertices[2].x), static_cast<int>(screenVertices[2].y), color, kFillModeWireFrame
	);
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[8] = {
		{aabb.min.x, aabb.min.y, aabb.min.z},
		{aabb.max.x, aabb.min.y, aabb.min.z},
		{aabb.max.x, aabb.max.y, aabb.min.z},
		{aabb.min.x, aabb.max.y, aabb.min.z},
		{aabb.min.x, aabb.min.y, aabb.max.z},
		{aabb.max.x, aabb.min.y, aabb.max.z},
		{aabb.max.x, aabb.max.y, aabb.max.z},
		{aabb.min.x, aabb.max.y, aabb.max.z}
	};
	Vector3 screenVertices[8];
	for (int i = 0; i < 8; ++i) {
		screenVertices[i] = Transform(vertices[i], viewProjectionMatrix * viewportMatrix);
	}
	int indices[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};
	for (int i = 0; i < 12; ++i) {
		Vector3 start = screenVertices[indices[i][0]];
		Vector3 end = screenVertices[indices[i][1]];
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
	}
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 normalizedV2 = v2.Normalized();
	float dotProduct = v1.Dot(normalizedV2);
	return normalizedV2 * dotProduct;
}

Vector3 CosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 toPoint = point - segment.origin;

	// clampするためのtを求める
	float t = toPoint.Dot(segment.diff) / segment.diff.Dot(segment.diff);
	t = max(0.0f, min(1.0f, t));

	// 最近点を求める
	return segment.origin + segment.diff * t;
}

bool isCollision(const Sphere& s1, const Sphere& s2) {
	Vector3 centerDiff = s2.center - s1.center;
	float radiusSum = s1.radius + s2.radius;
	return centerDiff.Dot(centerDiff) <= radiusSum * radiusSum;
}

bool isCollision(const Sphere& sphere, const Plane& plane) {
	float distance = sphere.center.Dot(plane.normal) - plane.distance;
	return fabs(distance) <= sphere.radius;
}

bool isCollision(const Sphere& sphere, const Segment& segment) {
	Vector3 closest = CosestPoint(sphere.center, segment);
	Vector3 diff = closest - sphere.center;
	return diff.Dot(diff) <= sphere.radius * sphere.radius;
}

bool isCollision(const Segment& segment, const Plane& plane) {
	float distanceOrigin = segment.origin.Dot(plane.normal) - plane.distance;
	float distanceEnd = (segment.origin + segment.diff).Dot(plane.normal) - plane.distance;
	return (distanceOrigin * distanceEnd <= 0.f);
}

bool isCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 edge1 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 edge2 = triangle.vertices[2] - triangle.vertices[0];
	Vector3 planeNormal = Vector3::Cross(edge1, edge2).Normalized();
	Plane plane{ planeNormal, planeNormal.Dot(triangle.vertices[0]) };
	if (!isCollision(segment, plane)) {
		return false;
	}
	Vector3 intersectionPoint = segment.origin + segment.diff * ((plane.distance - segment.origin.Dot(plane.normal)) / segment.diff.Dot(plane.normal));
	Vector3 c0 = intersectionPoint - triangle.vertices[0];
	Vector3 c1 = intersectionPoint - triangle.vertices[1];
	Vector3 c2 = intersectionPoint - triangle.vertices[2];
	return Vector3::Cross(edge1, c0).Dot(planeNormal) >= 0.f &&
		Vector3::Cross(triangle.vertices[2] - triangle.vertices[1], c1).Dot(planeNormal) >= 0.f &&
		Vector3::Cross(triangle.vertices[0] - triangle.vertices[2], c2).Dot(planeNormal) >= 0.f;
}

bool isCollision(const AABB& a, const AABB& b) {
	if((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) {
		return true;
	}
	return false;
}

bool isCollision(const Sphere& sphere, const AABB& aabb) {
	Vector3 closestPoint;
	closestPoint.x = max(aabb.min.x, min(sphere.center.x, aabb.max.x));
	closestPoint.y = max(aabb.min.y, min(sphere.center.y, aabb.max.y));
	closestPoint.z = max(aabb.min.z, min(sphere.center.z, aabb.max.z));
	float distance = Vector3::Length(closestPoint - sphere.center);
	return distance <= sphere.radius;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	Vector2i mousePos = { 0, 0 };
	Vector2i preMousePos = { 0, 0 };
	int mouseWheel = 0;

	Vector3 cameraPos = { 0.f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.f, 0.f };

	Vector3 pos1 = { 0, 0, 0 };

	AABB aabb1{
		.min{-0.5f, -0.5f, -0.5f},
		.max{0.0f, 0.0f, 0.0f}
	};

	Sphere sphere1{
		.center{0, 0, 0},
		.radius{0.5f}
	};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		preMousePos = mousePos;
		Novice::GetMousePosition(&mousePos.x, &mousePos.y);

		
		mouseWheel = Novice::GetWheel();

		///
		/// ↓更新処理ここから
		///
		
		if (Novice::IsPressMouse(1)) {
			cameraRotate.y += (mousePos.x - preMousePos.x) * 0.01f;
			cameraRotate.x += (mousePos.y - preMousePos.y) * 0.01f;
		}

		if (mouseWheel != 0) {
			Vector3 cameraDirection;
			cameraDirection.x = cosf(cameraRotate.x) * sinf(cameraRotate.y);
			cameraDirection.y = sinf(cameraRotate.x);
			cameraDirection.z = cosf(cameraRotate.x) * cosf(cameraRotate.y);
			cameraDirection = cameraDirection.Normalized();
			cameraPos = cameraPos + cameraDirection * float(mouseWheel) * 0.01f;
		}



		ImGui::Begin("window");
		ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.1f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("aabb1 min", &aabb1.min.x, 0.1f);
		ImGui::DragFloat3("aabb1 max", &aabb1.max.x, 0.1f);

		ImGui::DragFloat3("sphere1 center", &sphere1.center.x, 0.1f);
		ImGui::DragFloat("sphere1 radius", &sphere1.radius, 0.1f, 0.0f, 100.0f);
		ImGui::End();

		float temp;
		temp = min(aabb1.min.x, aabb1.max.x);
		aabb1.max.x = max(aabb1.min.x, aabb1.max.x);
		aabb1.min.x = temp;

		temp = min(aabb1.min.y, aabb1.max.y);
		aabb1.max.y = max(aabb1.min.y, aabb1.max.y);
		aabb1.min.y = temp;

		temp = min(aabb1.min.z, aabb1.max.z);
		aabb1.max.z = max(aabb1.min.z, aabb1.max.z);
		aabb1.min.z = temp;

		


		Matrix4x4 cameraMatrix = Matrix4x4::MakeAffineMatrix(Vector3(1, 1, 1), cameraRotate, cameraPos);
		Matrix4x4 viewMatrix = Matrix4x4::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, 640.0f / 360.0f, 0.1f, 100.0f);
		Matrix4x4 viewPortMatrix = Matrix4x4::MakeViewportMatrix(0, 0, 1280, 720, 0.0f, 1.0f);

		// Add this temporary debug code after line 300 (after creating matrices)
		Vector3 testPoint(0, 0, 0); // Origin
		Vector3 transformed = Transform(testPoint, viewMatrix * projectionMatrix * viewPortMatrix);
		Novice::ScreenPrintf(0, 20, "Origin (0,0,0) transforms to: %.1f, %.1f", transformed.x, transformed.y);

		Vector3 testPoint2(1, 0, 0); // X-axis
		Vector3 transformed2 = Transform(testPoint2, viewMatrix * projectionMatrix * viewPortMatrix);
		Novice::ScreenPrintf(0, 40, "Point (1,0,0) transforms to: %.1f, %.1f", transformed2.x, transformed2.y);


		bool collision = isCollision(sphere1, aabb1);

		///
		/// ↑更新処理ここまで
		///

		DrawGrid(viewPortMatrix, viewMatrix * projectionMatrix);

		DrawAABB(aabb1, viewMatrix * projectionMatrix, viewPortMatrix, collision ? 0xFF0000FF : 0xFFFFFFFF);
		DrawSphere(sphere1, viewMatrix * projectionMatrix, viewPortMatrix, collision ? 0xFF0000FF : 0xFFFFFFFF);



		//DrawSphere(sphere1, viewMatrix * projectionMatrix, viewPortMatrix, collision ? 0xFF0000FF : 0xFFFFFFFF);
		

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
