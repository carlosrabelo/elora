#pragma once

#include <core/optional.hpp>
#include <mesh/mesh.hpp>

#include <string>

namespace elora {

bool save_obj(const Mesh& mesh, const std::string& path);
Optional<Mesh> load_obj(const std::string& path);

}  // namespace elora
