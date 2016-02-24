#include <render/renderer.hpp>

#include <render/pipeline.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <vector>

namespace elora {
namespace {

int edge(int ax, int ay, int bx, int by, int px, int py) {
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

std::size_t pixel_index(int x, int y, int buffer_width) {
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(buffer_width) + static_cast<std::size_t>(x);
}

void fill_triangle(Engine& engine, std::vector<float>& depth, int bw, int bh, const ScreenTriangle& triangle,
                   std::uint32_t color, bool write_color) {
    const int area = edge(triangle.v0.x, triangle.v0.y, triangle.v1.x, triangle.v1.y, triangle.v2.x,
                          triangle.v2.y);
    if (area == 0) {
        return;
    }

    const int min_x = std::max(0, std::min(triangle.v0.x, std::min(triangle.v1.x, triangle.v2.x)));
    const int min_y = std::max(0, std::min(triangle.v0.y, std::min(triangle.v1.y, triangle.v2.y)));
    const int max_x = std::min(bw - 1, std::max(triangle.v0.x, std::max(triangle.v1.x, triangle.v2.x)));
    const int max_y = std::min(bh - 1, std::max(triangle.v0.y, std::max(triangle.v1.y, triangle.v2.y)));
    const float inv_area = 1.0f / static_cast<float>(area);

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            const int w0 = edge(triangle.v1.x, triangle.v1.y, triangle.v2.x, triangle.v2.y, x, y);
            const int w1 = edge(triangle.v2.x, triangle.v2.y, triangle.v0.x, triangle.v0.y, x, y);
            const int w2 = edge(triangle.v0.x, triangle.v0.y, triangle.v1.x, triangle.v1.y, x, y);
            const bool inside = (area > 0) ? (w0 >= 0 && w1 >= 0 && w2 >= 0)
                                           : (w0 <= 0 && w1 <= 0 && w2 <= 0);
            if (!inside) {
                continue;
            }
            const float z = (static_cast<float>(w0) * triangle.v0.z + static_cast<float>(w1) * triangle.v1.z +
                             static_cast<float>(w2) * triangle.v2.z) *
                            inv_area;
            const auto index = pixel_index(x, y, bw);
            if (z >= depth[index]) {
                continue;
            }
            depth[index] = z;
            if (write_color) {
                engine.put_pixel(x, y, color);
            }
        }
    }
}

void stroke_line(Engine& engine, const std::vector<float>& depth, int bw, int bh, const ScreenPoint& a,
                 const ScreenPoint& b, std::uint32_t color) {
    int x0 = a.x;
    int y0 = a.y;
    int x1 = b.x;
    int y1 = b.y;
    const int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    const int steps = std::max(dx, -dy);
    int i = 0;
    for (;;) {
        const float t = (steps == 0) ? 0.0f : static_cast<float>(i) / static_cast<float>(steps);
        const float z = a.z + (b.z - a.z) * t;
        if (x0 >= 0 && y0 >= 0 && x0 < bw && y0 < bh) {
            const auto index = pixel_index(x0, y0, bw);
            if (z <= depth[index] + 1.0e-3f) {
                engine.put_pixel(x0, y0, color);
            }
        }
        if (x0 == x1 && y0 == y1) {
            break;
        }
        const int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
        ++i;
    }
}

void stroke_triangle(Engine& engine, const std::vector<float>& depth, int bw, int bh,
                     const ScreenTriangle& triangle, std::uint32_t color) {
    stroke_line(engine, depth, bw, bh, triangle.v0, triangle.v1, color);
    stroke_line(engine, depth, bw, bh, triangle.v1, triangle.v2, color);
    stroke_line(engine, depth, bw, bh, triangle.v2, triangle.v0, color);
}

std::uint32_t shade_color(std::uint32_t color, float intensity) {
    if (intensity < 0.0f) {
        intensity = 0.0f;
    }
    if (intensity > 1.0f) {
        intensity = 1.0f;
    }
    const unsigned r = (color >> 16) & 0xffu;
    const unsigned g = (color >> 8) & 0xffu;
    const unsigned b = color & 0xffu;
    const auto channel = [intensity](unsigned value) {
        return static_cast<unsigned>(static_cast<float>(value) * intensity + 0.5f);
    };
    return (channel(r) << 16) | (channel(g) << 8) | channel(b);
}

float lambert(const Vector3D& normal, const Light& light) {
    Vector3D direction = light.direction.normalized();
    if (direction.length_squared() == 0.0f) {
        direction = {0, 0, 1};
    }
    float ndotl = normal.dot(direction);
    if (ndotl < 0.0f) {
        ndotl = 0.0f;
    }
    return light.ambient + light.diffuse * ndotl;
}

}  // namespace

void render(Engine& engine, const Mesh& mesh, const Camera& camera, float angle_x, float angle_y,
            float angle_z, Span<const std::uint32_t> face_colors, DrawMode mode,
            std::uint32_t clear_color, std::uint32_t stroke_color, const Light& light) {
    engine.clear(clear_color);
    const int bw = engine.buffer_width();
    const int bh = engine.buffer_height();
    if (bw <= 0 || bh <= 0 || (face_colors.empty() && mode != DrawMode::Wireframe)) {
        return;
    }

    std::vector<float> depth(static_cast<std::size_t>(bw) * static_cast<std::size_t>(bh),
                             std::numeric_limits<float>::infinity());
    std::vector<ScreenTriangle> visible;
    if (mode == DrawMode::Wireframe) {
        visible.reserve(mesh.triangles.size());
    }
    const bool write_color = mode == DrawMode::Solid && !face_colors.empty();

    for (std::size_t i = 0; i < mesh.triangles.size(); ++i) {
        const auto screen = transform_and_project(mesh, mesh.triangles[i], angle_x, angle_y, angle_z, camera,
                                                  bw, bh);
        if (!screen) {
            continue;
        }
        std::uint32_t color = write_color ? face_colors[i % face_colors.size()] : 0;
        if (write_color) {
            const Vector3D normal = rotated_face_normal(mesh, mesh.triangles[i], angle_x, angle_y, angle_z);
            color = shade_color(color, lambert(normal, light));
        }
        fill_triangle(engine, depth, bw, bh, *screen, color, write_color);
        if (mode == DrawMode::Wireframe) {
            visible.push_back(*screen);
        }
    }
    if (mode == DrawMode::Wireframe) {
        for (const auto& triangle : visible) {
            stroke_triangle(engine, depth, bw, bh, triangle, stroke_color);
        }
    }
}

void update(Engine& engine, const Mesh& mesh, const Camera& camera, float angle_x, float angle_y,
            float angle_z, Span<const std::uint32_t> face_colors, DrawMode mode,
            std::uint32_t clear_color, std::uint32_t stroke_color, const Light& light) {
    render(engine, mesh, camera, angle_x, angle_y, angle_z, face_colors, mode, clear_color, stroke_color,
           light);

    char fps_text[16];
    std::snprintf(fps_text, sizeof(fps_text), "FPS %d", static_cast<int>(engine.fps() + 0.5f));
    engine.draw_text(4, 4, fps_text, 0xf0f0f0, 2);
}

}  // namespace elora
