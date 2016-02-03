#pragma once

#include <mesh/mesh.hpp>

namespace elora {

inline Mesh make_cube(float half_extent = 1.0f) {
    Mesh mesh;
    const float h = half_extent;
    mesh.add_vertex({-h, -h, -h});
    mesh.add_vertex({h, -h, -h});
    mesh.add_vertex({h, h, -h});
    mesh.add_vertex({-h, h, -h});
    mesh.add_vertex({-h, -h, h});
    mesh.add_vertex({h, -h, h});
    mesh.add_vertex({h, h, h});
    mesh.add_vertex({-h, h, h});

    mesh.add_triangle(4, 5, 6);
    mesh.add_triangle(4, 6, 7);
    mesh.add_triangle(1, 0, 3);
    mesh.add_triangle(1, 3, 2);
    mesh.add_triangle(5, 1, 2);
    mesh.add_triangle(5, 2, 6);
    mesh.add_triangle(0, 4, 7);
    mesh.add_triangle(0, 7, 3);
    mesh.add_triangle(7, 6, 2);
    mesh.add_triangle(7, 2, 3);
    mesh.add_triangle(0, 1, 5);
    mesh.add_triangle(0, 5, 4);
    return mesh;
}

inline Mesh make_plane(float half_extent = 1.0f) {
    Mesh mesh;
    const float h = half_extent;
    mesh.add_vertex({-h, 0, -h});
    mesh.add_vertex({h, 0, -h});
    mesh.add_vertex({h, 0, h});
    mesh.add_vertex({-h, 0, h});
    mesh.add_triangle(0, 1, 2);
    mesh.add_triangle(0, 2, 3);
    return mesh;
}

inline Mesh make_tetrahedron(float size = 1.0f) {
    Mesh mesh;
    mesh.add_vertex({size, size, size});
    mesh.add_vertex({size, -size, -size});
    mesh.add_vertex({-size, size, -size});
    mesh.add_vertex({-size, -size, size});
    mesh.add_triangle(0, 1, 2);
    mesh.add_triangle(0, 2, 3);
    mesh.add_triangle(0, 3, 1);
    mesh.add_triangle(1, 3, 2);
    return mesh;
}

inline Mesh make_pyramid(float half_extent = 1.0f) {
    Mesh mesh;
    const float h = half_extent;
    mesh.add_vertex({-h, -h, -h});
    mesh.add_vertex({h, -h, -h});
    mesh.add_vertex({h, -h, h});
    mesh.add_vertex({-h, -h, h});
    mesh.add_vertex({0, h, 0});
    mesh.add_triangle(0, 2, 1);
    mesh.add_triangle(0, 3, 2);
    mesh.add_triangle(0, 1, 4);
    mesh.add_triangle(1, 2, 4);
    mesh.add_triangle(2, 3, 4);
    mesh.add_triangle(3, 0, 4);
    return mesh;
}

inline Mesh make_octahedron(float size = 1.0f) {
    Mesh mesh;
    mesh.add_vertex({size, 0, 0});
    mesh.add_vertex({-size, 0, 0});
    mesh.add_vertex({0, size, 0});
    mesh.add_vertex({0, -size, 0});
    mesh.add_vertex({0, 0, size});
    mesh.add_vertex({0, 0, -size});
    mesh.add_triangle(0, 2, 4);
    mesh.add_triangle(0, 4, 3);
    mesh.add_triangle(0, 3, 5);
    mesh.add_triangle(0, 5, 2);
    mesh.add_triangle(1, 4, 2);
    mesh.add_triangle(1, 3, 4);
    mesh.add_triangle(1, 5, 3);
    mesh.add_triangle(1, 2, 5);
    return mesh;
}

}  // namespace elora
