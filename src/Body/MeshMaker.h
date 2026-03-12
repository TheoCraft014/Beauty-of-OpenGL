//
// Created by theok on 25.11.2025.
//
#pragma once
#include <glm/glm.hpp>
#include <string>
#include "GL/glew.h"
#include <vector>
#include "core/gpu/vertex_format.h"
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace Colors2 {
    constexpr auto RED_ = glm::vec3(1, 0, 0);
    constexpr auto GREEN_ = glm::vec3(0, 1, 0);
    constexpr auto DARK_GREEN_ = glm::vec3(0, 0.4f, 0);
    constexpr auto BLUE_ = glm::vec3(0, 0, 1);
    constexpr auto LIGHT_BLUE_ = glm::vec3(0.2f, 0.6f, 1.f);
    constexpr auto SKY_BLUE_ = glm::vec3(0.52f, 0.8f, 0.92f);
    constexpr auto DARK_BLUE_ = glm::vec3(0.f, 0.f, 0.54f);
    constexpr auto BROWN_ = glm::vec3(0.8039, 0.5216, 0.2471);
    constexpr auto GRAY_ = glm::vec3(0.4f, 0.4f, 0.4f);
    constexpr auto YELLOW_ = glm::vec3(1.f, 1.f, 0);
    constexpr auto DARK_YELLOW_ = glm::vec3(0.8f, 0.8f, 0);
    constexpr auto ORANGE_ = glm::vec3(1, 0.647f, 0);
    constexpr auto DARK_PINK_ = glm::vec3(1, 0.08f, 0.57f);
    constexpr auto PURPLE_ = glm::vec3(0.55f, 0, 0.55f);
    constexpr auto WHITE_ = glm::vec3(1, 1, 1);
    constexpr auto BLACK_ = glm::vec3(0, 0, 0);
}

namespace Const2 {
    constexpr auto ORIGIN = glm::vec3(0, 0, 0);
    constexpr auto UP = glm::vec3(0, 1, 0);
    constexpr auto TerrainY = 0.1f;
    constexpr auto NUM_FRUNZE = 6;
    constexpr auto TREE_HEIGHT = 8.f;
    constexpr auto NUM_FIREFLIES = 30;
    constexpr auto TOWER_POS = glm::vec3(10, 0, 10);
}

namespace Maker {
    // preluat din lab2
    Mesh* MakeMesh(const std::string &name, const std::vector<VertexFormat> &vertices,
                        const std::vector<unsigned int> &indices, GLenum drawMode = GL_TRIANGLES);
}