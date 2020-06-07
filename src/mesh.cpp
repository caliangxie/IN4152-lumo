#include "mesh.h"
#include "disable_all_warnings.h"
DISABLE_WARNINGS_PUSH()
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/format.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <gsl/span>
DISABLE_WARNINGS_POP()
#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <stack>

static glm::mat4 assimpMatrix(const aiMatrix4x4& m)
{
    //float values[3][4] = {};
    glm::mat4 matrix;
    matrix[0][0] = m.a1;
    matrix[0][1] = m.b1;
    matrix[0][2] = m.c1;
    matrix[0][3] = m.d1;
    matrix[1][0] = m.a2;
    matrix[1][1] = m.b2;
    matrix[1][2] = m.c2;
    matrix[1][3] = m.d2;
    matrix[2][0] = m.a3;
    matrix[2][1] = m.b3;
    matrix[2][2] = m.c3;
    matrix[2][3] = m.d3;
    matrix[3][0] = m.a4;
    matrix[3][1] = m.b4;
    matrix[3][2] = m.c4;
    matrix[3][3] = m.d4;
    return matrix;
}

static glm::vec3 assimpVec(const aiVector3D& v)
{
    return glm::vec3(v.x, v.y, v.z);
}

static void centerAndScaleToUnitMesh(gsl::span<Vertex> vertices);

Mesh loadMesh(std::filesystem::path filePath, bool normalize)
{
    if (!std::filesystem::exists(filePath))
        throw MeshLoadingException(fmt::format("File {} does not exist", filePath.string().c_str()));

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.string().data(), aiProcess_GenSmoothNormals | aiProcess_Triangulate);

    if (scene == nullptr || scene->mRootNode == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
        throw MeshLoadingException(fmt::format("Assimp failed to load mesh file {}", filePath.string().c_str()));
    }

    Mesh out;
    std::stack<std::tuple<aiNode*, glm::mat4>> stack;
    stack.push({ scene->mRootNode, assimpMatrix(scene->mRootNode->mTransformation) });
    while (!stack.empty()) {
        auto [node, matrix] = stack.top();
        stack.pop();

        matrix *= assimpMatrix(node->mTransformation);
        const glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(matrix));

        for (unsigned i = 0; i < node->mNumMeshes; i++) {
            // Process subMesh
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            if (mesh->mNumVertices == 0 || mesh->mNumFaces == 0)
                std::cerr << "Empty mesh encountered" << std::endl;

            // Triangles
            const size_t indexOffset = out.vertices.size();
            for (unsigned j = 0; j < mesh->mNumFaces; j++) {
                const aiFace& face = mesh->mFaces[j];
                if (face.mNumIndices != 3) {
                    std::cerr << "Found a face which is not a triangle, discarding!" << std::endl;
                }

                auto aiIndices = face.mIndices;
                out.indices.push_back(static_cast<unsigned>(aiIndices[0] + indexOffset));
                out.indices.push_back(static_cast<unsigned>(aiIndices[1] + indexOffset));
                out.indices.push_back(static_cast<unsigned>(aiIndices[2] + indexOffset));
            }

            // Vertices
            for (unsigned j = 0; j < mesh->mNumVertices; j++) {
                const glm::vec3 pos = matrix * glm::vec4(assimpVec(mesh->mVertices[j]), 1.0f);
                const glm::vec3 normal = normalMatrix * assimpVec(mesh->mNormals[j]);
                out.vertices.push_back(Vertex { pos, normal });
            }
        }

        for (unsigned i = 0; i < node->mNumChildren; i++) {
            stack.push({ node->mChildren[i], matrix });
        }
    }
    importer.FreeScene();

    if (normalize)
        centerAndScaleToUnitMesh(out.vertices);
    return out;
}

size_t getClosestVertexIndex(const Mesh& mesh, const glm::vec3& pos)
{
    // TODO: replace by std::min_element() + std::distance
    size_t out { 0xFFFFFFFF };
    float closest = std::numeric_limits<float>::max();
    for (size_t i = 0; i < mesh.vertices.size(); i++) {
        const float d = glm::length(mesh.vertices[i].pos - pos);
        if (d < closest) {
            out = i;
            closest = d;
        }
    }
    return out;
}

static void centerAndScaleToUnitMesh(gsl::span<Vertex> vertices)
{
#ifdef MODERN_CPP
    const glm::vec3 center = std::transform_reduce(std::begin(vertices), std::end(vertices), glm::vec3(0), std::plus<glm::vec3>(), [](const Vertex& v) { return v.pos; }) / static_cast<float>(vertices.size());
    const float maxD = std::transform_reduce(
        std::begin(vertices), std::end(vertices),
        0.0f,
        [](float lhs, float rhs) { return std::max(lhs, rhs); },
        [=](const Vertex& v) { return glm::length(v.pos - center); });
    std::transform(std::begin(vertices), std::end(vertices), std::begin(vertices),
        [=](Vertex v) {
            v.pos = (v.pos - center) / maxD;
            return v;
        });
#else
    std::vector<glm::vec3> positions(vertices.size());
    std::transform(std::begin(vertices), std::end(vertices), std::begin(positions), [](const Vertex& v) { return v.pos; });

    const glm::vec3 center = std::accumulate(std::begin(positions), std::end(positions), glm::vec3(0.0f)) / static_cast<float>(positions.size());
    float maxD = 0.0f;
    for (const Vertex& v : vertices)
        maxD = std::max(glm::length(v.pos - center), maxD);
    std::transform(std::begin(vertices), std::end(vertices), std::begin(vertices),
        [=](Vertex v) {
            v.pos = (v.pos - center) / maxD;
            return v;
        });
#endif
}
