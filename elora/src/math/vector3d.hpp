#pragma once

#include <cmath>

namespace elora {

struct Vector3D {
    float x{0};
    float y{0};
    float z{0};

    Vector3D() = default;
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3D operator+(const Vector3D& other) const { return {x + other.x, y + other.y, z + other.z}; }

    Vector3D operator-(const Vector3D& other) const { return {x - other.x, y - other.y, z - other.z}; }

    Vector3D operator-() const { return {-x, -y, -z}; }

    Vector3D operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }

    Vector3D operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }

    bool operator==(const Vector3D& other) const { return x == other.x && y == other.y && z == other.z; }

    bool operator!=(const Vector3D& other) const { return !(*this == other); }

    float length_squared() const { return x * x + y * y + z * z; }

    float length() const { return std::sqrt(length_squared()); }

    float dot(const Vector3D& other) const { return x * other.x + y * other.y + z * other.z; }

    Vector3D cross(const Vector3D& other) const {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    Vector3D normalized() const {
        const float len = length();
        if (len == 0.0f) {
            return {};
        }
        return *this / len;
    }

    Vector3D rotated_x(float radians) const {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return {x, y * c - z * s, y * s + z * c};
    }

    Vector3D rotated_y(float radians) const {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return {x * c + z * s, y, -x * s + z * c};
    }

    Vector3D rotated_z(float radians) const {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return {x * c - y * s, x * s + y * c, z};
    }

    Vector3D rotated(float radians_x, float radians_y, float radians_z) const {
        return rotated_x(radians_x).rotated_y(radians_y).rotated_z(radians_z);
    }
};

inline Vector3D operator*(float scalar, const Vector3D& v) { return v * scalar; }

}  // namespace elora
