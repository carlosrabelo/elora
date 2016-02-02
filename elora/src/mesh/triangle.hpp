#pragma once

#include <cstddef>

namespace elora {

struct Triangle {
    std::size_t v0{0};
    std::size_t v1{0};
    std::size_t v2{0};

    bool operator==(const Triangle& other) const {
        return v0 == other.v0 && v1 == other.v1 && v2 == other.v2;
    }

    bool operator!=(const Triangle& other) const { return !(*this == other); }
};

}  // namespace elora
