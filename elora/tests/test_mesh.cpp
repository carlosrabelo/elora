#include <catch.hpp>
#include <io/obj.hpp>
#include <math/vector3d.hpp>
#include <mesh/mesh.hpp>
#include <mesh/primitives.hpp>
#include <mesh/triangle.hpp>

#include <cstdio>
#include <fstream>
#include <string>

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

TEST_CASE("make_cube has eight vertices and twelve triangles") {
    const elora::Mesh cube = elora::make_cube();
    REQUIRE(cube.vertices.size() == 8);
    REQUIRE(cube.triangles.size() == 12);
}

TEST_CASE("make_cube triangles index the eight vertices") {
    const elora::Mesh cube = elora::make_cube();
    for (const auto& t : cube.triangles) {
        REQUIRE(t.v0 < 8);
        REQUIRE(t.v1 < 8);
        REQUIRE(t.v2 < 8);
    }
}

TEST_CASE("make_plane has two triangles") {
    const elora::Mesh plane = elora::make_plane();
    REQUIRE(plane.vertices.size() == 4);
    REQUIRE(plane.triangles.size() == 2);
}

TEST_CASE("make_tetrahedron has four triangles") {
    const elora::Mesh tet = elora::make_tetrahedron();
    REQUIRE(tet.vertices.size() == 4);
    REQUIRE(tet.triangles.size() == 4);
}

TEST_CASE("make_pyramid has six triangles") {
    const elora::Mesh pyramid = elora::make_pyramid();
    REQUIRE(pyramid.vertices.size() == 5);
    REQUIRE(pyramid.triangles.size() == 6);
}

TEST_CASE("make_octahedron has eight triangles") {
    const elora::Mesh octa = elora::make_octahedron();
    REQUIRE(octa.vertices.size() == 6);
    REQUIRE(octa.triangles.size() == 8);
}

TEST_CASE("OBJ save and load round-trips a cube") {
    const std::string path = "/tmp/elora_test_cube.obj";
    REQUIRE(elora::save_obj(elora::make_cube(), path));
    const auto loaded = elora::load_obj(path);
    REQUIRE(loaded.has_value());
    REQUIRE(loaded->vertices.size() == 8);
    REQUIRE(loaded->triangles.size() == 12);
    REQUIRE(loaded->vertices[0] == elora::Vector3D{-1, -1, -1});
    std::remove(path.c_str());
}

TEST_CASE("assets cube.obj loads") {
    auto loaded = elora::load_obj("assets/cube.obj");
    if (!loaded) {
        loaded = elora::load_obj("../assets/cube.obj");
    }
    REQUIRE(loaded.has_value());
    REQUIRE(loaded->vertices.size() == 8);
    REQUIRE(loaded->triangles.size() == 12);
}

TEST_CASE("assets teapot.obj loads") {
    auto loaded = elora::load_obj("assets/teapot.obj");
    if (!loaded) {
        loaded = elora::load_obj("../assets/teapot.obj");
    }
    REQUIRE(loaded.has_value());
    REQUIRE(loaded->vertices.size() == 800);
    REQUIRE(loaded->triangles.size() == 1024);
}

TEST_CASE("OBJ loader triangulates quads and vt/vn indices") {
    const std::string path = "/tmp/elora_test_quad.obj";
    {
        std::ofstream out(path);
        out << "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\n";
        out << "f 1/1/1 2/2/2 3/3/3 4/4/4\n";
    }
    const auto loaded = elora::load_obj(path);
    REQUIRE(loaded.has_value());
    REQUIRE(loaded->triangles.size() == 2);
    REQUIRE(loaded->triangles[0] == elora::Triangle{0, 1, 2});
    REQUIRE(loaded->triangles[1] == elora::Triangle{0, 2, 3});
    std::remove(path.c_str());
}
