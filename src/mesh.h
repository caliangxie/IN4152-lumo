#pragma once
#include "disable_all_warnings.h"
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <vector>

struct MeshLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};
Mesh loadMesh(std::filesystem::path filePath, bool normalize = false);
size_t getClosestVertexIndex(const Mesh& mesh, const glm::vec3& pos);
