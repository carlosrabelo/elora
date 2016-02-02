#include <catch.hpp>
#include <math/vector3d.hpp>

TEST_CASE("Vector3D defaults to zero") {
    elora::Vector3D v;
    REQUIRE(v == elora::Vector3D{0, 0, 0});
}

TEST_CASE("Vector3D stores components") {
    const elora::Vector3D v{1, 2, 3};
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
}

TEST_CASE("Vector3D adds and subtracts") {
    const elora::Vector3D a{1, 2, 3};
    const elora::Vector3D b{4, 5, 6};
    REQUIRE(a + b == elora::Vector3D{5, 7, 9});
    REQUIRE(b - a == elora::Vector3D{3, 3, 3});
    REQUIRE(-a == elora::Vector3D{-1, -2, -3});
}

TEST_CASE("Vector3D scales") {
    const elora::Vector3D v{1, 2, 3};
    REQUIRE(v * 2 == elora::Vector3D{2, 4, 6});
    REQUIRE(2 * v == elora::Vector3D{2, 4, 6});
    REQUIRE(v / 2 == elora::Vector3D{0.5f, 1, 1.5f});
}

TEST_CASE("Vector3D dot and cross") {
    const elora::Vector3D i{1, 0, 0};
    const elora::Vector3D j{0, 1, 0};
    REQUIRE(i.dot(j) == 0);
    REQUIRE(i.cross(j) == elora::Vector3D{0, 0, 1});
}

TEST_CASE("Vector3D length and normalized") {
    const elora::Vector3D v{3, 4, 0};
    REQUIRE(v.length_squared() == 25);
    REQUIRE(v.length() == Approx(5));
    const elora::Vector3D n = v.normalized();
    REQUIRE(n.x == Approx(0.6f));
    REQUIRE(n.y == Approx(0.8f));
    REQUIRE(n.z == 0);
}

TEST_CASE("Vector3D normalized of zero is zero") {
    REQUIRE(elora::Vector3D{}.normalized() == elora::Vector3D{});
}

TEST_CASE("Vector3D rotated_x by quarter turn") {
    const elora::Vector3D v = elora::Vector3D{0, 1, 0}.rotated_x(1.57079632f);
    REQUIRE(v.x == Approx(0).margin(0.0001f));
    REQUIRE(v.y == Approx(0).margin(0.0001f));
    REQUIRE(v.z == Approx(1).margin(0.0001f));
}

TEST_CASE("Vector3D rotated_y by quarter turn") {
    const elora::Vector3D v = elora::Vector3D{1, 0, 0}.rotated_y(1.57079632f);
    REQUIRE(v.x == Approx(0).margin(0.0001f));
    REQUIRE(v.y == Approx(0).margin(0.0001f));
    REQUIRE(v.z == Approx(-1).margin(0.0001f));
}

TEST_CASE("Vector3D rotated_z by quarter turn") {
    const elora::Vector3D v = elora::Vector3D{1, 0, 0}.rotated_z(1.57079632f);
    REQUIRE(v.x == Approx(0).margin(0.0001f));
    REQUIRE(v.y == Approx(1).margin(0.0001f));
    REQUIRE(v.z == Approx(0).margin(0.0001f));
}
