#pragma once

#include <core/span.hpp>
#include <engine/engine.hpp>
#include <math/camera.hpp>
#include <math/vector3d.hpp>
#include <mesh/mesh.hpp>

#include <cstdint>

namespace elora {

enum class DrawMode { Solid, Wireframe };

struct Light {
    Vector3D direction{0.35f, 0.8f, 0.5f};
    float ambient{0.32f};
    float diffuse{0.68f};
};

void render(Engine& engine, const Mesh& mesh, const Camera& camera, float angle_x, float angle_y,
            float angle_z, Span<const std::uint32_t> face_colors, DrawMode mode = DrawMode::Solid,
            std::uint32_t clear_color = 0x101018, std::uint32_t stroke_color = 0xc8c8d0,
            const Light& light = {});

// Compose a full frame in the back buffer (mesh + HUD). present() is the only path to the window.
void update(Engine& engine, const Mesh& mesh, const Camera& camera, float angle_x, float angle_y,
            float angle_z, Span<const std::uint32_t> face_colors, DrawMode mode = DrawMode::Solid,
            std::uint32_t clear_color = 0x101018, std::uint32_t stroke_color = 0xc8c8d0,
            const Light& light = {});

}  // namespace elora
