#include <io/obj.hpp>

#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace elora {
namespace {

int parse_face_index(const std::string& token, int vertex_count) {
    const auto slash = token.find('/');
    const std::string index_text = token.substr(0, slash);
    if (index_text.empty()) {
        return -1;
    }
    int index = std::stoi(index_text);
    if (index < 0) {
        index = vertex_count + index + 1;
    }
    return index - 1;
}

}  // namespace

bool save_obj(const Mesh& mesh, const std::string& path) {
    std::ofstream out(path);
    if (!out) {
        return false;
    }

    out << "# elora mesh\n";
    for (const auto& v : mesh.vertices) {
        out << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    for (const auto& t : mesh.triangles) {
        out << "f " << (t.v0 + 1) << " " << (t.v1 + 1) << " " << (t.v2 + 1) << "\n";
    }
    return static_cast<bool>(out);
}

Optional<Mesh> load_obj(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return {};
    }

    Mesh mesh;
    try {
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream tokens(line);
            std::string tag;
            tokens >> tag;
            if (tag == "v") {
                float x = 0;
                float y = 0;
                float z = 0;
                tokens >> x >> y >> z;
                mesh.add_vertex({x, y, z});
            } else if (tag == "f") {
                std::vector<int> indices;
                std::string token;
                const int vertex_count = static_cast<int>(mesh.vertices.size());
                while (tokens >> token) {
                    const int index = parse_face_index(token, vertex_count);
                    if (index < 0 || index >= vertex_count) {
                        return {};
                    }
                    indices.push_back(index);
                }
                if (indices.size() < 3) {
                    return {};
                }
                for (std::size_t i = 1; i + 1 < indices.size(); ++i) {
                    mesh.add_triangle(static_cast<std::size_t>(indices[0]),
                                      static_cast<std::size_t>(indices[i]),
                                      static_cast<std::size_t>(indices[i + 1]));
                }
            }
        }
    } catch (const std::exception&) {
        return {};
    }

    if (mesh.vertices.empty() || mesh.triangles.empty()) {
        return {};
    }
    return mesh;
}

}  // namespace elora
