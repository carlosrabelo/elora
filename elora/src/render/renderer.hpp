#pragma once

#include <core/span.hpp>
#include <engine/engine.hpp>
#include <math/camera.hpp>
#include <mesh/mesh.hpp>

#include <cstdint>

namespace elora {

enum class DrawMode { Solid, Wireframe };

void render(Engine& engine, const Mesh& mesh, const Camera& camera, float angle_x, float angle_y,
            float angle_z, Span<const std::uint32_t> face_colors, DrawMode mode = DrawMode::Solid,
            std::uint32_t clear_color = 0x101018, std::uint32_t stroke_color = 0xc8c8d0);

// Compose a full frame in the back buffer (mesh + HUD). present() is the only path to the window.
void update(Engine& engine, const Mesh& mesh, const Camera& camera, float angle_x, float angle_y,
            float angle_z, Span<const std::uint32_t> face_colors, DrawMode mode = DrawMode::Solid,
            std::uint32_t clear_color = 0x101018, std::uint32_t stroke_color = 0xc8c8d0);

}  // namespace elora
