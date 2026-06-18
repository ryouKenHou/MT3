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

struct OBB {
	Vector3 center;
	Vector3 orientation[3];
	Vector3 size;

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

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[8];
	for (int i = 0; i < 8; ++i) {
		Vector3 localVertex = {
			(i & 1 ? 1 : -1) * obb.size.x ,
			(i & 2 ? 1 : -1) * obb.size.y ,
			(i & 4 ? 1 : -1) * obb.size.z 
		};
		vertices[i] = obb.center +
			obb.orientation[0] * localVertex.x +
			obb.orientation[1] * localVertex.y +
			obb.orientation[2] * localVertex.z;
	}
	Vector3 screenVertices[8];
	for (int i = 0; i < 8; ++i) {
		screenVertices[i] = Transform(vertices[i], viewProjectionMatrix * viewportMatrix);
	}
	int indices[12][2] = {
		{0, 1}, {5, 7}, {2, 0}, {3, 2},
		{4, 5}, {3, 1}, {6, 7}, {6, 4},
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

bool isCollision(const Segment& segment, const AABB& aabb) {
	float tmin = 0.0f;  // Start of segment
	float tmax = 1.0f;  // End of segment

	// Check each axis
	for (int i = 0; i < 3; ++i) {
		float origin_i = (i == 0) ? segment.origin.x : (i == 1) ? segment.origin.y : segment.origin.z;
		float diff_i = (i == 0) ? segment.diff.x : (i == 1) ? segment.diff.y : segment.diff.z;
		float min_i = (i == 0) ? aabb.min.x : (i == 1) ? aabb.min.y : aabb.min.z;
		float max_i = (i == 0) ? aabb.max.x : (i == 1) ? aabb.max.y : aabb.max.z;

		if (fabs(diff_i) < 1e-6f) {
			// Segment is parallel to this axis
			// Check if origin is outside the box on this axis
			if (origin_i < min_i || origin_i > max_i) {
				return false;
			}
		}
		else {
			// Calculate intersection t values for this axis
			float t1 = (min_i - origin_i) / diff_i;
			float t2 = (max_i - origin_i) / diff_i;

			float tNear = min(t1, t2);
			float tFar = max(t1, t2);

			tmin = max(tmin, tNear);
			tmax = min(tmax, tFar);

			if (tmin > tmax) {
				return false;
			}
		}
	}

	return true;
}

bool isCollision(const Line& line, const AABB& aabb) {
	float tmin = -FLT_MAX;  // Start of line
	float tmax = FLT_MAX;   // End of line

	for (int i = 0; i < 3; ++i) {
		float origin_i = (i == 0) ? line.origin.x : (i == 1) ? line.origin.y : line.origin.z;
		float diff_i = (i == 0) ? line.diff.x : (i == 1) ? line.diff.y : line.diff.z;
		float min_i = (i == 0) ? aabb.min.x : (i == 1) ? aabb.min.y : aabb.min.z;
		float max_i = (i == 0) ? aabb.max.x : (i == 1) ? aabb.max.y : aabb.max.z;

		if (fabs(diff_i) < 1e-6f) {
			// Line is parallel to this axis
			if (origin_i < min_i || origin_i > max_i) {
				return false;
			}
		}
		else {
			float t1 = (min_i - origin_i) / diff_i;
			float t2 = (max_i - origin_i) / diff_i;
			float tNear = min(t1, t2);
			float tFar = max(t1, t2);
			tmin = max(tmin, tNear);
			tmax = min(tmax, tFar);
			if (tmin > tmax) {
				return false;
			}
		}
	}
	return true;
}

bool isCollision(const Ray& ray, const AABB& aabb) {
	float tmin = 0.0f;  // Start of ray
	float tmax = FLT_MAX;   // End of ray
	for (int i = 0; i < 3; ++i) {
		float origin_i = (i == 0) ? ray.origin.x : (i == 1) ? ray.origin.y : ray.origin.z;
		float diff_i = (i == 0) ? ray.diff.x : (i == 1) ? ray.diff.y : ray.diff.z;
		float min_i = (i == 0) ? aabb.min.x : (i == 1) ? aabb.min.y : aabb.min.z;
		float max_i = (i == 0) ? aabb.max.x : (i == 1) ? aabb.max.y : aabb.max.z;
		if (fabs(diff_i) < 1e-6f) {
			// Ray is parallel to this axis
			if (origin_i < min_i || origin_i > max_i) {
				return false;
			}
		}
		else {
			float t1 = (min_i - origin_i) / diff_i;
			float t2 = (max_i - origin_i) / diff_i;
			float tNear = min(t1, t2);
			float tFar = max(t1, t2);
			tmin = max(tmin, tNear);
			tmax = min(tmax, tFar);
			if (tmin > tmax) {
				return false;
			}
		}
	}
	return true;
}

bool isCollision(const Sphere& sphere, const OBB& obb) {
	Matrix4x4 obbWorldMatrix = Matrix4x4::MakeAffineMatrix(Vector3(1, 1, 1), Vector3(0, 0, 0), obb.center);
	Matrix4x4 obbInverseMatrix = Matrix4x4::Inverse(obbWorldMatrix);

	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbInverseMatrix);

	AABB aabbOBBLocal{.min = obb.size * -1, .max = obb.size};
	Sphere sphereInOBBLocal{ .center = centerInOBBLocalSpace, .radius = sphere.radius };

	return isCollision(sphereInOBBLocal, aabbOBBLocal);
}

bool isCollision(const Line& line, const OBB& obb) {
	Matrix4x4 obbWorldMatrix = Matrix4x4::MakeAffineMatrix(Vector3(1, 1, 1), Vector3(0, 0, 0), obb.center);
	Matrix4x4 obbInverseMatrix = Matrix4x4::Inverse(obbWorldMatrix);
	Vector3 localOrigin = Transform(line.origin, obbInverseMatrix);
	Vector3 localEnd = Transform(line.origin + line.diff, obbInverseMatrix);

	AABB aabbOBBLocal{ .min = obb.size * -1, .max = obb.size };

	Line lineInOBBLocal{ .origin = localOrigin, .diff = localEnd - localOrigin };
	return isCollision(lineInOBBLocal, aabbOBBLocal);
}

bool isCollision(const Ray& ray, const OBB& obb) {
	Matrix4x4 obbWorldMatrix = Matrix4x4::MakeAffineMatrix(Vector3(1, 1, 1), Vector3(0, 0, 0), obb.center);
	Matrix4x4 obbInverseMatrix = Matrix4x4::Inverse(obbWorldMatrix);
	Vector3 rayOriginInOBBLocalSpace = Transform(ray.origin, obbInverseMatrix);
	Vector3 rayDiffInOBBLocalSpace = Transform(ray.origin + ray.diff, obbInverseMatrix) - rayOriginInOBBLocalSpace;
	Ray rayInOBBLocal{ .origin = rayOriginInOBBLocalSpace, .diff = rayDiffInOBBLocalSpace };
	AABB aabbOBBLocal{ .min = obb.size * -1, .max = obb.size };
	return isCollision(rayInOBBLocal, aabbOBBLocal);
}

bool isCollision(const Segment& segment, const OBB& obb) {
	Matrix4x4 obbWorldMatrix = Matrix4x4::MakeAffineMatrix(Vector3(1, 1, 1), Vector3(0, 0, 0), obb.center);
	Matrix4x4 obbInverseMatrix = Matrix4x4::Inverse(obbWorldMatrix);

	Vector3 segmentOriginInOBBLocalSpace = Transform(segment.origin, obbInverseMatrix);
	Vector3 segmentDiffInOBBLocalSpace = Transform(segment.origin + segment.diff, obbInverseMatrix) - segmentOriginInOBBLocalSpace;

	Segment segmentInOBBLocal{ .origin = segmentOriginInOBBLocalSpace, .diff = segmentDiffInOBBLocalSpace };
	AABB aabbOBBLocal{ .min = obb.size * -1, .max = obb.size };
	return isCollision(segmentInOBBLocal, aabbOBBLocal);
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
	//int mouseWheel = 0;

	Vector3 cameraPos = { 0.f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.f, 0.f };

	Vector3 pos1 = { 0, 0, 0 };

	Vector3 rotate{ 0.f,0.f,0.f };
	OBB obb{
		.center = { -1.f, 0.f, 0.f },
		.orientation = {Vector3(1.f, 0.f, 0.f),
						Vector3(0.f, 1.f, 0.f),
						Vector3(0.f, 0.f, 1.f)},
		.size = { 0.5f, 0.5f, 0.5f }
	};

	Segment segment{
		.origin = { 0.f, 0.f, 0.f },
		.diff = { 1.f, 1.f, 1.f }
	};


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
		ImGui::Begin("window");
		ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.1f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);

		ImGui::DragFloat3("segment.origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);

		ImGui::DragFloat3("obb.center", &obb.center.x, 0.01f);
		ImGui::DragFloat3("obb.size", &obb.size.x, 0.01f);
		ImGui::DragFloat3("rotation", &rotate.x, 0.01f);

		ImGui::End();
		Matrix4x4 rotationMatrix = Matrix4x4::MakeRotationXMatrix(rotate.x) * Matrix4x4::MakeRotationYMatrix(rotate.y) * Matrix4x4::MakeRotationZMatrix(rotate.z);
		obb.orientation[0].x = rotationMatrix.m[0][0];
		obb.orientation[0].y = rotationMatrix.m[0][1];
		obb.orientation[0].z = rotationMatrix.m[0][2];

		obb.orientation[1].x = rotationMatrix.m[1][0];
		obb.orientation[1].y = rotationMatrix.m[1][1];
		obb.orientation[1].z = rotationMatrix.m[1][2];

		obb.orientation[2].x = rotationMatrix.m[2][0];
		obb.orientation[2].y = rotationMatrix.m[2][1];
		obb.orientation[2].z = rotationMatrix.m[2][2];


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


		bool collision = isCollision(segment, obb);

		///
		/// ↑更新処理ここまで
		///

		DrawGrid(viewPortMatrix, viewMatrix * projectionMatrix);

		DrawOBB(obb, viewMatrix * projectionMatrix, viewPortMatrix, collision ? 0xFF0000FF : 0xFFFFFFFF);
		
		Vector3 segmentEnd = segment.origin + segment.diff;
		Vector3 screenStart = Transform(segment.origin, viewMatrix * projectionMatrix * viewPortMatrix);
		Vector3 screenEnd = Transform(segmentEnd, viewMatrix * projectionMatrix * viewPortMatrix);
		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), collision ? 0xFF0000FF : 0xFFFFFFFF);

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
