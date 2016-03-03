#pragma once

#include <core/optional.hpp>
#include <math/camera.hpp>
#include <math/vector3d.hpp>
#include <mesh/mesh.hpp>
#include <mesh/triangle.hpp>

#include <algorithm>

namespace elora {

struct ScreenTriangle {
    ScreenPoint v0;
    ScreenPoint v1;
    ScreenPoint v2;
};

inline int screen_area(int x0, int y0, int x1, int y1, int x2, int y2) {
    return (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
}

inline float screen_area(float x0, float y0, float x1, float y1, float x2, float y2) {
    return (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
}

inline bool is_back_facing(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2,
                           const Vector3D& eye) {
    const Vector3D normal = (p1 - p0).cross(p2 - p0);
    const Vector3D centroid = (p0 + p1 + p2) * (1.0f / 3.0f);
    return normal.dot(eye - centroid) <= 0.0f;
}

inline bool is_off_screen(const ScreenTriangle& triangle, int buffer_width, int buffer_height) {
    const int min_x = std::min(triangle.v0.x, std::min(triangle.v1.x, triangle.v2.x));
    const int max_x = std::max(triangle.v0.x, std::max(triangle.v1.x, triangle.v2.x));
    const int min_y = std::min(triangle.v0.y, std::min(triangle.v1.y, triangle.v2.y));
    const int max_y = std::max(triangle.v0.y, std::max(triangle.v1.y, triangle.v2.y));
    return max_x < 0 || min_x >= buffer_width || max_y < 0 || min_y >= buffer_height;
}

inline Optional<ScreenTriangle> project_triangle(const Vector3D& p0, const Vector3D& p1,
                                                 const Vector3D& p2, const Camera& camera,
                                                 int buffer_width, int buffer_height) {
    const auto s0 = camera.project(p0, buffer_width, buffer_height);
    const auto s1 = camera.project(p1, buffer_width, buffer_height);
    const auto s2 = camera.project(p2, buffer_width, buffer_height);
    if (!s0 || !s1 || !s2) {
        return {};
    }
    if (is_back_facing(p0, p1, p2, camera.position())) {
        return {};
    }
    const ScreenTriangle screen{*s0, *s1, *s2};
    if (is_off_screen(screen, buffer_width, buffer_height)) {
        return {};
    }
    return screen;
}

inline Vector3D rotated_face_normal(const Mesh& mesh, const Triangle& triangle, float angle_x,
                                   float angle_y, float angle_z) {
    const auto p = mesh.positions(triangle);
    const Vector3D p0 = p[0].rotated(angle_x, angle_y, angle_z);
    const Vector3D p1 = p[1].rotated(angle_x, angle_y, angle_z);
    const Vector3D p2 = p[2].rotated(angle_x, angle_y, angle_z);
    return (p1 - p0).cross(p2 - p0).normalized();
}

inline Optional<ScreenTriangle> transform_and_project(const Mesh& mesh, const Triangle& triangle,
                                                           float angle_x, float angle_y, float angle_z,
                                                           const Camera& camera, int buffer_width,
                                                           int buffer_height) {
    const auto p = mesh.positions(triangle);
    return project_triangle(p[0].rotated(angle_x, angle_y, angle_z), p[1].rotated(angle_x, angle_y, angle_z),
                            p[2].rotated(angle_x, angle_y, angle_z), camera, buffer_width, buffer_height);
}

}  // namespace elora
