#include <catch.hpp>
#include <math/camera.hpp>
#include <mesh/mesh.hpp>
#include <mesh/primitives.hpp>
#include <render/pipeline.hpp>

TEST_CASE("screen_area is positive for clockwise screen winding") {
    REQUIRE(elora::screen_area(0, 0, 10, 0, 0, 10) == 100);
}

TEST_CASE("screen_area is negative for counter-clockwise screen winding") {
    REQUIRE(elora::screen_area(0, 0, 0, 10, 10, 0) == -100);
}

TEST_CASE("rotated_face_normal of the cube +Z face points forward") {
    const elora::Mesh cube = elora::make_cube();
    const elora::Vector3D n = elora::rotated_face_normal(cube, cube.triangles[0], 0, 0, 0);
    REQUIRE(n.x == Approx(0).margin(0.0001f));
    REQUIRE(n.y == Approx(0).margin(0.0001f));
    REQUIRE(n.z == Approx(1).margin(0.0001f));
}

TEST_CASE("project_triangle keeps a front-facing cube face") {
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const auto screen = elora::transform_and_project(cube, cube.triangles[0], 0, 0, 0, camera, 160, 120);
    REQUIRE(screen.has_value());
}

TEST_CASE("project_triangle culls a back-facing cube face") {
    const elora::Mesh cube = elora::make_cube();
    const elora::Camera camera{{0, 0, -3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const auto screen = elora::transform_and_project(cube, cube.triangles[0], 0, 0, 0, camera, 160, 120);
    REQUIRE_FALSE(screen.has_value());
}

TEST_CASE("project_triangle rejects a triangle behind the camera") {
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const auto screen = elora::project_triangle({-1, -1, 5}, {1, -1, 5}, {0, 1, 5}, camera, 160, 120);
    REQUIRE_FALSE(screen.has_value());
}

TEST_CASE("project_triangle rejects a triangle that misses the buffer") {
    const elora::Camera camera{{0, 0, 3}, {0, 0, 0}, {0, 1, 0}, 60.0f};
    const auto screen = elora::project_triangle({-20, -20, 0}, {-18, -20, 0}, {-19, -18, 0}, camera, 160, 120);
    REQUIRE_FALSE(screen.has_value());
}
