#include <catch.hpp>
#include <math/camera.hpp>
#include <math/vector3d.hpp>

TEST_CASE("Camera projects the look target to the screen center") {
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const auto p = camera.project({0, 0, 0}, 160, 120);
    REQUIRE(p.has_value());
    REQUIRE(p->x == 80);
    REQUIRE(p->y == 60);
    REQUIRE(p->z == Approx(3.0f));
}

TEST_CASE("Camera rejects points behind the near plane") {
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    REQUIRE_FALSE(camera.project({0, 0, 5}, 160, 120).has_value());
}

TEST_CASE("Camera position and fov are writable") {
    elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    camera.set_position({1, 2, 4});
    camera.set_fov_degrees(45.0f);
    REQUIRE(camera.position() == elora::Vector3D{1, 2, 4});
    REQUIRE(camera.fov_degrees() == 45.0f);
}

TEST_CASE("Camera projects a right-hand point to the right of center") {
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const auto p = camera.project({1, 0, 0}, 160, 120);
    REQUIRE(p.has_value());
    REQUIRE(p->x > 80);
}
