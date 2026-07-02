#pragma once
#include <cmath>

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3 operator+(const Vector3& other) const {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator-(const Vector3& other) const {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 operator*(float scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 operator/(float scalar) const {
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	float Length() const {
		return sqrt(x * x + y * y + z * z);
	}

	float Dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3 Normalized() const {
		float length = Length();
		if (length == 0) {
			return Vector3(0, 0, 0); // Avoid division by zero
		}
		return Vector3(x / length, y / length, z / length);
	}

	void Normalize() {
		float length = Length();
		if (length == 0) {
			return; // Avoid division by zero
		}
		x /= length;
		y /= length;
		z /= length;
	}


	// Static member functions for operations between two vectors
	static Vector3 Add(const Vector3& v1, const Vector3& v2) {
		return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	static float Dot(const Vector3& v1, const Vector3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	static Vector3 Multiply(float scalar, const Vector3& v) {
		return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
	}

	static Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
		return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	static float Length(const Vector3& v) {
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	static Vector3 Normalized(const Vector3& v) {
		float length = Length(v);
		if (length == 0) {
			return Vector3(0, 0, 0); // Avoid division by zero
		}
		return Vector3(v.x / length, v.y / length, v.z / length);
	}

	static Vector3 Cross(const Vector3& v1, const Vector3& v2) {
		return Vector3(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);
	}

	
};

inline Vector3 operator*(const float s, const Vector3 v) {
	return v * s;
}

inline Vector3 operator-(const Vector3 v) {
	return v * -1.f ;
}
inline Vector3 operator+( const Vector3 v) {
	return v;
}

inline Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	return v1 + ((v2 - v1) *  t);
}



