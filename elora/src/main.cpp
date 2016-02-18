#include <core/optional.hpp>
#include <engine/engine.hpp>
#include <io/obj.hpp>
#include <math/camera.hpp>
#include <math/vector3d.hpp>
#include <mesh/mesh.hpp>
#include <render/renderer.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

elora::Optional<elora::Mesh> load_asset(const std::string& name) {
    elora::Optional<elora::Mesh> mesh = elora::load_obj("assets/" + name);
    if (mesh) {
        return mesh;
    }
    return elora::load_obj("../assets/" + name);
}

}  // namespace

int main() {
    const auto loaded = load_asset("teapot.obj");
    if (!loaded) {
        std::cerr << "error: could not load assets/teapot.obj\n";
        return 1;
    }
    const elora::Mesh mesh = *loaded;

    elora::Engine engine{"3D Demo", 1024, 768, 2, 2};
    engine.init();

    const elora::Vector3D camera_start{1.8f, 1.4f, 3.0f};
    const elora::Vector3D camera_target{0, 0, 0};
    constexpr float camera_fov = 55.0f;
    elora::Camera camera{camera_start, camera_target, {0, 1, 0}, camera_fov};
    constexpr std::uint32_t face_colors[] = {0xe07070, 0x70a070, 0x7070e0, 0xd0c060, 0x60c0c0, 0xc070b0};
    constexpr float speed_x = 0.7f;
    constexpr float speed_y = 0.9f;
    constexpr float speed_z = 0.4f;

    float angle_x = 0;
    float angle_y = 0;
    float angle_z = 0;
    bool space_was_down = false;
    bool t_was_down = false;
    bool p_was_down = false;
    bool wireframe = false;
    bool auto_rotate = true;
    auto last = std::chrono::steady_clock::now();

    const bool ok = engine.run([&] {
        const auto now = std::chrono::steady_clock::now();
        const float dt = std::chrono::duration<float>(now - last).count();
        last = now;
        const bool space_down = engine.is_key_down(elora::Key::Space);
        if (space_down && !space_was_down) {
            camera.set_position(camera_start);
            camera.set_target(camera_target);
            camera.set_fov_degrees(camera_fov);
            angle_x = 0;
            angle_y = 0;
            angle_z = 0;
        } else if (auto_rotate) {
            angle_x += speed_x * dt;
            angle_y += speed_y * dt;
            angle_z += speed_z * dt;
        }
        space_was_down = space_down;

        const bool t_down = engine.is_key_down(elora::Key::T);
        if (t_down && !t_was_down) {
            wireframe = !wireframe;
        }
        t_was_down = t_down;

        const bool p_down = engine.is_key_down(elora::Key::P);
        if (p_down && !p_was_down) {
            auto_rotate = !auto_rotate;
        }
        p_was_down = p_down;

        elora::Vector3D pos = camera.position();
        constexpr float camera_speed = 2.0f;
        if (engine.is_key_down(elora::Key::A) || engine.is_key_down(elora::Key::Left)) {
            pos.x -= camera_speed * dt;
        }
        if (engine.is_key_down(elora::Key::D) || engine.is_key_down(elora::Key::Right)) {
            pos.x += camera_speed * dt;
        }
        if (engine.is_key_down(elora::Key::F)) {
            pos.y -= camera_speed * dt;
        }
        if (engine.is_key_down(elora::Key::R)) {
            pos.y += camera_speed * dt;
        }
        if (engine.is_key_down(elora::Key::S) || engine.is_key_down(elora::Key::Down)) {
            pos.z -= camera_speed * dt;
        }
        if (engine.is_key_down(elora::Key::W) || engine.is_key_down(elora::Key::Up)) {
            pos.z += camera_speed * dt;
        }
        if ((pos - camera.target()).length() > 0.35f) {
            camera.set_position(pos);
        }

        const auto mode = wireframe ? elora::DrawMode::Wireframe : elora::DrawMode::Solid;
        elora::render(engine, mesh, camera, angle_x, angle_y, angle_z, face_colors, mode);
    });

    engine.shutdown();
    return ok ? 0 : 1;
}
