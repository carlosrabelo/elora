#pragma once

#include <core/optional.hpp>
#include <math/vector3d.hpp>

#include <cmath>

namespace elora {

struct ScreenPoint {
    int x{0};
    int y{0};
    float z{0};
};

class Camera {
public:
    Camera(Vector3D position, Vector3D target, Vector3D up, float fov_degrees, float near_plane = 0.1f)
        : position_(position),
          target_(target),
          up_(up),
          fov_degrees_(fov_degrees),
          near_plane_(near_plane) {}

    const Vector3D& position() const { return position_; }
    const Vector3D& target() const { return target_; }
    const Vector3D& up() const { return up_; }
    float fov_degrees() const { return fov_degrees_; }
    float near_plane() const { return near_plane_; }

    void set_position(Vector3D position) { position_ = position; }
    void set_target(Vector3D target) { target_ = target; }
    void set_up(Vector3D up) { up_ = up; }
    void set_fov_degrees(float fov_degrees) { fov_degrees_ = fov_degrees; }
    void set_near_plane(float near_plane) { near_plane_ = near_plane; }

    Optional<ScreenPoint> project(const Vector3D& world, int buffer_width, int buffer_height) const {
        if (buffer_width <= 0 || buffer_height <= 0) {
            return {};
        }

        const Vector3D forward = (target_ - position_).normalized();
        if (forward.length_squared() == 0.0f) {
            return {};
        }

        Vector3D right = forward.cross(up_);
        if (right.length_squared() == 0.0f) {
            const Vector3D fallback = std::fabs(forward.y) > 0.9f ? Vector3D{0, 0, 1} : Vector3D{0, 1, 0};
            right = forward.cross(fallback);
        }
        right = right.normalized();
        const Vector3D cam_up = right.cross(forward).normalized();

        const Vector3D delta = world - position_;
        const float cam_x = delta.dot(right);
        const float cam_y = delta.dot(cam_up);
        const float cam_z = delta.dot(forward);
        if (cam_z <= near_plane_) {
            return {};
        }

        constexpr float pi = 3.14159265f;
        const float fov_rad = fov_degrees_ * (pi / 180.0f);
        const float f = 1.0f / std::tan(fov_rad * 0.5f);
        const float aspect = static_cast<float>(buffer_width) / static_cast<float>(buffer_height);
        const float ndc_x = (cam_x * f / aspect) / cam_z;
        const float ndc_y = (cam_y * f) / cam_z;

        const int x = static_cast<int>((ndc_x * 0.5f + 0.5f) * static_cast<float>(buffer_width));
        const int y = static_cast<int>((1.0f - (ndc_y * 0.5f + 0.5f)) * static_cast<float>(buffer_height));
        return ScreenPoint{x, y, cam_z};
    }

private:
    Vector3D position_;
    Vector3D target_;
    Vector3D up_;
    float fov_degrees_;
    float near_plane_;
};

}  // namespace elora
