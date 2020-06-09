#include "disable_all_warnings.h"
DISABLE_WARNINGS_PUSH()
#include <GL/glew.h>
// Include GLEW before GLFW
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
// Library for loading an image
#include <stb_image.h>
DISABLE_WARNINGS_POP()

// Header for camera structure/functions
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "window.h"

#include <iostream>
#include <vector>

// Configuration
constexpr int WIDTH = 512;
constexpr int HEIGHT = 512;


int main()
{
    Window window { glm::ivec2(WIDTH, HEIGHT), "Paper Presentation Demo", false };

    Camera camera { &window, glm::vec3(1.0f, 0.0f, 0.0f), -glm::vec3(1.0f, 0.0f, 0.0f) }; // -glm::vec3(1.2f, 1.1f, 0.9f)  forward direction is -x
    constexpr float fov = glm::pi<float>() / 4.0f;
    constexpr float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspect, 0.1f, 30.0f);

    // === Modify for exercise 1 ===
    // Key handle function
    window.registerKeyCallback([&](int key, int /* scancode */, int action, int /* mods */) {
        switch (key) {
        case GLFW_KEY_1:
            break;
        case GLFW_KEY_2:
            break;
        default:
            break;
        }
        });

    const Shader approxShader = ShaderBuilder()
        .addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl")
        .addStage(GL_FRAGMENT_SHADER, "shaders/approx_frag.glsl").build();
    const Shader interpShader = ShaderBuilder()
        .addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl")
        .addStage(GL_FRAGMENT_SHADER, "shaders/interp_frag.glsl").build();

    // === Load a texture for exercise 5 ===
    // Create Texture
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("resources/shape4.png", &texWidth, &texHeight, &texChannels, 3);

    GLuint texDrawing;
    glCreateTextures(GL_TEXTURE_2D, 1, &texDrawing);
    glTextureStorage2D(texDrawing, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(texDrawing, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texDrawing, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texDrawing, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texDrawing, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texDrawing, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load mesh from disk.
    const Mesh mesh = loadMesh("resources/quad.obj");

    // Create Element(Index) Buffer Object and Vertex Buffer Objects.
    GLuint ibo;
    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, static_cast<GLsizeiptr>(mesh.indices.size() * sizeof(decltype(Mesh::indices)::value_type)), mesh.indices.data(), 0);

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(Vertex)), mesh.vertices.data(), 0);

    // Bind vertex data to shader inputs using their index (location).
    // These bindings are stored in the Vertex Array Object.
    GLuint vao;
    glCreateVertexArrays(1, &vao);

    // The indicies (pointing to vertices) should be read from the index buffer.
    glVertexArrayElementBuffer(vao, ibo);

    // The position and normal vectors should be retrieved from the specified Vertex Buffer Object.
    // The stride is the distance in bytes between vertices. We use the offset to point to the normals
    // instead of the positions.
    glVertexArrayVertexBuffer(vao, 0, vbo, offsetof(Vertex, pos), sizeof(Vertex));
    glVertexArrayVertexBuffer(vao, 1, vbo, offsetof(Vertex, normal), sizeof(Vertex));
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    GLuint texApprox;
    glCreateTextures(GL_TEXTURE_2D, 1, &texApprox);
    glTextureStorage2D(texApprox, 1, GL_RGB8, WIDTH, HEIGHT);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texApprox, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texApprox, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texApprox, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texApprox, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // === Create framebuffer for extra texture ===
    GLuint framebuffer;
    glCreateFramebuffers(1, &framebuffer);
    glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texApprox, 0);

    // Main loop
    while (!window.shouldClose()) {
        window.updateInput();
        const glm::mat4 mvp = mainProjectionMatrix * camera.viewMatrix(); // Assume model matrix is identity.
        // First stage, normal approximation
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            // Bind the shader
            approxShader.bind();

            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

            // Bind vertex data
            glBindVertexArray(vao);

            // Bind the drawing texture to texture slot 0
            GLuint texture_unit = 0;
            glActiveTexture(GL_TEXTURE0 + texture_unit);
            glBindTexture(GL_TEXTURE_2D, texDrawing);
            glUniform1i(2, texture_unit);

            // Set viewport size
            glViewport(0, 0, WIDTH, HEIGHT);

            // Clear the framebuffer to black and depth to maximum value
            glClearDepth(1.0f);
            glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            // Execute draw command
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Second stage, normal interpolation
        {
            // Bind the shader
            interpShader.bind();

            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

            // Bind vertex data
            glBindVertexArray(vao);

            // Bind the drawing texture to texture slot 0
            GLuint texture_unit = 0;
            glActiveTexture(GL_TEXTURE0 + texture_unit);
            glBindTexture(GL_TEXTURE_2D, texApprox);
            glUniform1i(2, texture_unit);

            // Set viewport size
            glViewport(0, 0, WIDTH, HEIGHT);

            // Clear the framebuffer to black and depth to maximum value
            glClearDepth(1.0f);
            glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            // Execute draw command
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
        }

        // Present result to the screen.
        window.swapBuffers();
    }

    // Be a nice citizen and clean up after yourself.
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texDrawing);
    glDeleteTextures(1, &texApprox);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}