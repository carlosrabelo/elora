#pragma once

#include <math/vector3d.hpp>
#include <mesh/triangle.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace elora {

struct Mesh {
    std::vector<Vector3D> vertices;
    std::vector<Triangle> triangles;

    std::size_t add_vertex(Vector3D vertex) {
        vertices.push_back(vertex);
        return vertices.size() - 1;
    }

    void add_triangle(std::size_t v0, std::size_t v1, std::size_t v2) {
        triangles.push_back({v0, v1, v2});
    }

    std::array<Vector3D, 3> positions(const Triangle& triangle) const {
        return {vertices[triangle.v0], vertices[triangle.v1], vertices[triangle.v2]};
    }
};

}  // namespace elora
