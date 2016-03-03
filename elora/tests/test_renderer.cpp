#include <catch.hpp>
#include <engine/engine.hpp>
#include <mesh/mesh.hpp>
#include <mesh/primitives.hpp>
#include <render/renderer.hpp>

#include <cstdint>

namespace {
const elora::Light kUnlit{{0, 0, 1}, 1.0f, 0.0f};
}

TEST_CASE("render clears and rasterizes a visible cube") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0xff0000};
    elora::render(engine, cube, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000, 0xc8c8d0, kUnlit);
    REQUIRE(engine.pixel(80, 60) == 0xff0000);
    REQUIRE(engine.pixel(0, 0) == 0x000000);
}

TEST_CASE("render still clears when face colors are empty") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    engine.put_pixel(0, 0, 0xffffff);
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    elora::render(engine, cube, camera, 0, 0, 0, {}, elora::DrawMode::Solid, 0x112233);
    REQUIRE(engine.pixel(0, 0) == 0x112233);
    REQUIRE(engine.pixel(80, 60) == 0x112233);
}

TEST_CASE("render hides a farther triangle behind a nearer one") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    elora::Mesh mesh;
    mesh.add_vertex({-1.5f, -1.5f, 0});
    mesh.add_vertex({1.5f, -1.5f, 0});
    mesh.add_vertex({0, 1.5f, 0});
    mesh.add_triangle(0, 1, 2);
    mesh.add_vertex({-0.3f, -0.3f, 0.8f});
    mesh.add_vertex({0.3f, -0.3f, 0.8f});
    mesh.add_vertex({0, 0.3f, 0.8f});
    mesh.add_triangle(3, 4, 5);
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0xff0000, 0x00ff00};
    elora::render(engine, mesh, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000, 0xc8c8d0, kUnlit);
    REQUIRE(engine.pixel(80, 60) == 0x00ff00);
}

TEST_CASE("render hides a farther triangle even when it is submitted first") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    elora::Mesh mesh;
    mesh.add_vertex({-0.3f, -0.3f, 0.8f});
    mesh.add_vertex({0.3f, -0.3f, 0.8f});
    mesh.add_vertex({0, 0.3f, 0.8f});
    mesh.add_triangle(0, 1, 2);
    mesh.add_vertex({-1.5f, -1.5f, 0});
    mesh.add_vertex({1.5f, -1.5f, 0});
    mesh.add_vertex({0, 1.5f, 0});
    mesh.add_triangle(3, 4, 5);
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0x00ff00, 0xff0000};
    elora::render(engine, mesh, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000, 0xc8c8d0, kUnlit);
    REQUIRE(engine.pixel(80, 60) == 0x00ff00);
}

TEST_CASE("update rasterizes the mesh then draws the FPS overlay") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0xff0000};
    elora::update(engine, cube, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000, 0xc8c8d0, kUnlit);
    REQUIRE(engine.pixel(80, 60) == 0xff0000);
    REQUIRE(engine.pixel(4, 4) == 0xf0f0f0);
}

TEST_CASE("render shades a face facing the light brighter than one facing away") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    elora::Mesh mesh;
    mesh.add_vertex({-1.5f, -1.5f, 0});
    mesh.add_vertex({1.5f, -1.5f, 0});
    mesh.add_vertex({0, 1.5f, 0});
    mesh.add_triangle(0, 1, 2);
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0xffffff};
    const elora::Light front{{0, 0, 1}, 0.0f, 1.0f};
    const elora::Light back{{0, 0, -1}, 0.0f, 1.0f};
    elora::render(engine, mesh, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000, 0xc8c8d0, front);
    REQUIRE(engine.pixel(80, 60) == 0xffffff);
    elora::render(engine, mesh, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000, 0xc8c8d0, back);
    REQUIRE(engine.pixel(80, 60) == 0x000000);
}

TEST_CASE("render default light darkens a cube face that is not head-on") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0xff0000};
    elora::render(engine, cube, camera, 0, 0, 0, colors, elora::DrawMode::Solid, 0x000000);
    const std::uint32_t pixel = engine.pixel(80, 60);
    REQUIRE((pixel & 0xff0000) != 0);
    REQUIRE((pixel & 0xff0000) < 0xff0000);
}

TEST_CASE("render wireframe leaves the interior of a cube unfilled") {
    elora::Engine engine{"test", 160, 120};
    engine.init();
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const std::uint32_t colors[] = {0xff0000};
    elora::render(engine, cube, camera, 0, 0, 0, colors, elora::DrawMode::Wireframe, 0x000000, 0xffffff);
    REQUIRE(engine.pixel(80, 60) == 0x000000);
}
