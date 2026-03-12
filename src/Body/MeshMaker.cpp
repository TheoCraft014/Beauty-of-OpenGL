//
// Created by theok on 25.11.2025.
//
#include "MeshMaker.h"

#include <iostream>
#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"


// preluat din lab2
namespace Maker {
    Mesh* MakeMesh(const std::string &name, const std::vector<VertexFormat> &vertices,
                        const std::vector<unsigned int> &indices, GLenum drawmode) {

        // VAO
        unsigned int VAO = 0;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // VBO
        unsigned int VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

        // IBO
        unsigned int IBO = 0;
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

        // trimit pozitia vertexului
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), nullptr);

        // trimit normala vertexului
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

        // trimit coordonatele texturii
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
                                (void*)(2 * sizeof(glm::vec3)));

        // trimit culoarea vertexului
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
                                (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

        // VAO scos
        glBindVertexArray(0);

        Mesh *mesh = new Mesh(name);
        mesh->SetDrawMode(drawmode);
        mesh->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));

        return mesh;
    }
}