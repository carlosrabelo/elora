#include <catch.hpp>
#include <math/vector3d.hpp>
#include <mesh/mesh.hpp>
#include <mesh/triangle.hpp>

TEST_CASE("Triangle stores vertex indices") {
    const elora::Triangle t{1, 2, 3};
    REQUIRE(t.v0 == 1);
    REQUIRE(t.v1 == 2);
    REQUIRE(t.v2 == 3);
}

TEST_CASE("Mesh add_vertex returns the index") {
    elora::Mesh mesh;
    REQUIRE(mesh.add_vertex({0, 0, 0}) == 0);
    REQUIRE(mesh.add_vertex({1, 0, 0}) == 1);
    REQUIRE(mesh.vertices.size() == 2);
}

TEST_CASE("Mesh add_triangle stores indices") {
    elora::Mesh mesh;
    mesh.add_vertex({0, 0, 0});
    mesh.add_vertex({1, 0, 0});
    mesh.add_vertex({0, 1, 0});
    mesh.add_triangle(0, 1, 2);
    REQUIRE(mesh.triangles.size() == 1);
    REQUIRE(mesh.triangles[0] == elora::Triangle{0, 1, 2});
}

TEST_CASE("Mesh positions resolves a triangle") {
    elora::Mesh mesh;
    mesh.add_vertex({0, 0, 0});
    mesh.add_vertex({1, 0, 0});
    mesh.add_vertex({0, 1, 0});
    mesh.add_triangle(0, 1, 2);
    const auto p = mesh.positions(mesh.triangles[0]);
    REQUIRE(p[0] == elora::Vector3D{0, 0, 0});
    REQUIRE(p[1] == elora::Vector3D{1, 0, 0});
    REQUIRE(p[2] == elora::Vector3D{0, 1, 0});
}
